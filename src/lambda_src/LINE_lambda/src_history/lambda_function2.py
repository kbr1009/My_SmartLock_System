import json
import os
import urllib.request
import logging
import boto3
logger = logging.getLogger()
logger.setLevel(logging.INFO)

LINE_CHANNEL_ACCESS_TOKEN   = os.environ['LINE_CHANNEL_ACCESS_TOKEN']

REQUEST_URL = 'https://api.line.me/v2/bot/message/reply'
REQUEST_METHOD = 'POST'
REQUEST_HEADERS = {
        'Authorization': 'Bearer ' + LINE_CHANNEL_ACCESS_TOKEN,
        'Content-Type': 'application/json'
        }

def lambda_handler(event, context):

    logger.info(event)
    dynamodb = boto3.resource('dynamodb')
    table = dynamoDB.Table('lock_status')

    for message_event in json.loads(event['body'])['events']:
        key_info = message_event['message']['text']
        reply_token = message_event['replyToken']

    if key_info == "解錠":
        res = table.update_item(
                Key={
                    'status':1,
                    }
                )
        message = "解錠完了"

    elif key_info == "施錠":
        res = table.update_item(
                Key={
                    'status':0,
                    }
                )
        message = "施錠完了"

    else:
        message = "Fuck you!"

    print(res)

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
    return 0
