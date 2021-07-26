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


def operation_put(status, timestamp):
    putResponse = table.put_item(
        Item={
            'id': 1,
            'status': status,
            'timestamp': timestamp
        }
    )
    if putResponse['ResponseMetadata']['HTTPStatusCode'] != 200:
        print(putResponse)
    else:
       print('PUT Successed.')
    return putResponse


def operation_get():
    getResponse = table.get_item(
        Key={
            'id': 1
        }
    )
    get_status = getResponse['Item']['status']
    return get_status


def lambda_handler(event, context):
    logger.info(event)
    diff_time = 9
    date_info = datetime.datetime.utcnow() + datetime.timedelta(hours = diff_time)
    timestamp = date_info.strftime('%m月%d日 %H:%M:%S')
    latest_status = operation_get()

    for message_event in json.loads(event['body'])['events']:
        key_info = message_event['message']['text']
        reply_token = message_event['replyToken']

    if key_info == "解錠":
        if latest_status == "Open":
            message = "もう鍵空いてるよ"
        else:
            status = "Open"
            message = "駆動時刻："+ timestamp + "\nステータス[ Open ]"
    elif key_info == "施錠":
        if latest_status == "Open":
            status = "Close"
            message = "駆動時刻："+ timestamp + "\nステータス[ Close ]"
        else:
            message = "もう閉まってるよ"
    else:
        message = "Fuck you!"

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
    return operation_put(status, timestamp)