import json
import os
import urllib.request
import boto3


LINE_CHANNEL_ACCESS_TOKEN   = os.environ['LINE_CHANNEL_ACCESS_TOKEN']
REQUEST_URL = 'https://api.line.me/v2/bot/message/reply'
REQUEST_METHOD = 'POST'
REQUEST_HEADERS = {
        'Authorization': 'Bearer ' + LINE_CHANNEL_ACCESS_TOKEN,
        'Content-Type': 'application/json'
        }
dynamoDB = boto3.resource('dynamodb')
table = dynamoDB.Table('IotData')


def operation_get():
    getResponse = table.get_item(
        Key={
            'id': 1
        }
    )
    get_reply_token = getResponse['Item']['replyToken']
    return get_reply_token

def lambda_handler(event, context):
    reply_token = operation_get()
    message = "完了！"

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
    return "Succeeeded."