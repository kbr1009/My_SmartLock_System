import json
import os
import urllib.request
import logging
import boto3
import datetime

import decimal
from boto3.dynamodb.conditions import Key

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
table = dynamoDB.Table('MyIotData')


def operation_put(timestamp, status):
    putResponse = table.put_item(
        Item={
            'timestamp': timestamp,
            'lock_status': status
        }
    )
    if putResponse['ResponseMetadata']['HTTPStatusCode'] != 200:
        print(putResponse)
    else:
       print('PUT Successed.')
    return putResponse


def operation_select():
    res = table.query(
            KeyConditionExpression=Key('timestamp')
            )
    for latest_item in json.load(max(res["Items"], key=(lambda x:x["timestamp"]))):
        latest_status = latest_item['timestamp']['lock_status']
    
    return latest_status
    

def lambda_handler(event, context):
    logger.info(event)
    diff_time = 9
    date_info = datetime.datetime.utcnow() + datetime.timedelta(hours = diff_time)
    timestamp = date_info.strftime('%Y年%m月%d日 %H:%M:%S')

    late_st = operation_select

    for message_event in json.loads(event['body'])['events']:
        key_info = message_event['message']['text']
        reply_token = message_event['replyToken']

    if key_info == "解錠":
        status = "開"
        message = "駆動時刻："+ timestamp + " statsu : open "+ late_st

    elif key_info == "施錠":
        status = "閉"
        message = "駆動時刻："+ timestamp + " statsu : close"

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
    return operation_put(timestamp, status)