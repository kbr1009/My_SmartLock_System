import json
import boto3
 

dynamoDB = boto3.resource('dynamodb')
table = dynamoDB.Table('IotData')
iot = boto3.client('iot-data')


def operation_get():
    getResponse = table.get_item(
        Key={
            'id': 1
        }
    )
    get_status = getResponse['Item']['status']
    return get_status


def lambda_handler(event, context):
    topic = 'reo/smartlock'
    latest_status = operation_get()

    if  latest_status == "Open":
        payload = {
            "switch": "Unlock"
            }
    elif latest_status =="Close":
        payload = {
            "switch": "Lock"
            }

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
