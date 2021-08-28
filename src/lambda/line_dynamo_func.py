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
table = dynamoDB.Table('IotData')


def operation_put(reply_token):
    putResponse = table.put_item(
        Item={
            'id': 1,
            'replyToken': reply_token
        }
    )
    return putResponse


def operation_get():
    getResponse = table.get_item(
        Key={
            'id': 2
        }
    )
    get_status = getResponse['Item']['status']
    return get_status


def pub_iotcore(payload):
    iot = boto3.client('iot-data')
    topic = '$aws/things/reo_smartkey/shadow/key_order/update'
    try:
        iot.publish(
            topic=topic,
            qos=0,
            payload=json.dumps(payload, ensure_ascii=False)
        )
        return "Succeeeded."

    except Exception as e:
        print(e)
        return "Shit! Failed."

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

    for message_event in json.loads(event['body'])['events']:
        key_info = message_event['message']['text']
        reply_token = message_event['replyToken']
    
    latest_status = operation_get()

    if key_info == "Fire!":
        if latest_status == "Open":
            payload = {"Order": "Lock"}
        if latest_status == "Close":
            payload = {"Order": "Unlock"}
    elif key_info == "reboot":
        payload = {"Order": "Reboot"}
    else:
        message = "Fuck you!"
        reply_func(reply_token, message)

    return operation_put(reply_token), pub_iotcore(payload)