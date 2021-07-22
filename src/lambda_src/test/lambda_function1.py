import json
import os
import urllib.request
import logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)

LINE_CHANNEL_ACCESS_TOKEN   = os.environ['LINE_CHANNEL_ACCESS_TOKEN']

REQUEST_URL = 'https://api.line.me/v2/bot/message/reply'
REQUEST_METHOD = 'POST'
REQUEST_HEADERS = {
    'Authorization': 'Bearer ' + LINE_CHANNEL_ACCESS_TOKEN,
    'Content-Type': 'application/json'
}
REQUEST_MESSAGE = [
    {
        'type': 'text',
        'text': 'Fuck you!'
    }
]

def lambda_handler(event, context):
    logger.info(event)
    params = {
        'replyToken': json.loads(event['body'])['events'][0]['replyToken'],
        'messages': REQUEST_MESSAGE
    }
    request = urllib.request.Request(
        REQUEST_URL, 
        json.dumps(params).encode('utf-8'), 
        method=REQUEST_METHOD, 
        headers=REQUEST_HEADERS
        )
    response = urllib.request.urlopen(request, timeout=10)
    return 0
