import json
import os
import urllib.request
import logging
import boto3
import datetime


logger = logging.getLogger()
logger.setLevel(logging.INFO)
LINE_CHANNEL_ACCESS_TOKEN   = os.environ['LINE_CHANNEL_ACCESS_TOKEN']
REQUEST_URL = 'https://api.line.me/v2/bot/message/reply'
REQUEST_METHOD = 'POST'
REQUEST_HEADERS = {
        'Authorization': 'Bearer ' + LINE_CHANNEL_ACCESS_TOKEN,
        'Content-Type': 'application/json'
        }
dynamoDB = boto3.resource('dynamodb')
table = dynamoDB.Table('IotData')


def operation_put(status, timestamp, reply_token):
    putResponse = table.put_item(
        Item={
            'id': 1,
            'status': status,
            'timestamp': timestamp,
            'replyToken': reply_token
        }
    )
    return putResponse


def operation_get():
    getResponse = table.get_item(
        Key={
            'id': 1
        }
    )
    get_status = getResponse['Item']['status']
    get_timestamp = getResponse['Item']['timestamp']
    return get_status, get_timestamp

def reply_func(reply_token, message):
    params = {
            'replyToken': reply_token,
            'messages': [
                {
                    'type': 'text',
                    'text': message,
                    }
                ]   
            }
    request = urllib.request.Request(
            REQUEST_URL, 
            json.dumps(params).encode('utf-8'), 
            method=REQUEST_METHOD, 
            headers=REQUEST_HEADERS
            )
    response = urllib.request.urlopen(request, timeout=10)


def lambda_handler(event, context):
    logger.info(event)
    diff_time = 9
    date_info = datetime.datetime.utcnow() + datetime.timedelta(hours = diff_time)
    timestamp = date_info.strftime('%m月%d日 %H:%M:%S')
    latest_data = operation_get()
    latest_status = latest_data[0]
    latest_timestamp = latest_data[1]

    for message_event in json.loads(event['body'])['events']:
        key_info = message_event['message']['text']
        reply_token = message_event['replyToken']
    
    if key_info == "解錠":

        if latest_status == "Open":
            message = latest_timestamp + "に開けてあるよ"
            reply_func(reply_token, message)
        else:
            status = "Open"

    elif key_info == "施錠":

        if latest_status == "Open":
            status = "Close"

        else:
            message = latest_timestamp + "に閉めてあるよ"
            reply_func(reply_token, message)

    else:
        message = "Fuck you!"
        reply_func(reply_token, message)

    return operation_put(status, timestamp, reply_token)