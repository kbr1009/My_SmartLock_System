import json
import os
import urllib.request
import boto3
import datetime


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
    
def status_put(status):
    putResponse = table.put_item(
        Item={
            'id': 2,
            'status': status
        }
    )
    return putResponse
    
def reboottime_put(timestamp):
    putResponse = table.put_item(
        Item={
            'id': 3,
            'reboot_time': timestamp
        }
    )
    return putResponse

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
    reply_token = operation_get()
    pubdata = str(event['Status'])
    
    if pubdata == "Locked":
        status = "Close"
        message = "Locked"

    if pubdata == "Unlocked":
        status = "Open"
        message = "Unlocked"
    
    if pubdata == "Rebooted":
        diff_time = 9
        date_info = datetime.datetime.utcnow() + datetime.timedelta(hours = diff_time)
        timestamp = date_info.strftime('%m月%d日 %H:%M:%S')
        message = timestamp + "に再起動済"
        reboottime_put(timestamp)
    
    return reply_func(reply_token, message), status_put(status)