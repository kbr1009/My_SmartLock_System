import json
import boto3

iot = boto3.client('iot-data')

#関数を定義
def lambda_handler(event, context):

    #トピックを指定
    topic = '$aws/things/ESP32/shadow/update'
    #メッセージの内容
    #形式通りのJsonでお願いします
    payload = {"state": {"desired": {"led": "on"}}}
    try:
        #メッセージをPublish
        iot.publish(
            topic=topic,
            qos=0,
            payload=json.dumps(payload, ensure_ascii=False)
        )

        return "Succeeeded."

    except Exception:
        return "Failed."
