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

dynamoDB = boto3.resource('dynamodb')
table = dynamoDB.Table('MyLock')

def operation_put(lock_status):
    putResponse = table.put_item(
        Item={
            'id': 1,
            'lock_status': lock_status
        }
    )
    if putResponse['ResponseMetadata']['HTTPStatusCode'] != 200:
        print(putResponse)
    else:
       print('PUT Successed.')
    return putResponse

def lambda_handler(event, context):
    logger.info(event)


    for message_event in json.loads(event['body'])['events']:
        key_info = message_event['message']['text']
        reply_token = message_event['replyToken']

    if key_info == "解錠":
        lock_status = "開"
        message = "鍵あけたよ"

    elif key_info == "施錠":
        lock_status = "閉"
        message = "鍵かけたよ"

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
    return operation_put(lock_status)
