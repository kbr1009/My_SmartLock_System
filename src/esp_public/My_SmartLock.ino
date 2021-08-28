#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ssl_client.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <Servo.h>
  
char *ssid = "******";
char *password = "******";
  
const char *endpoint = "******";

const int port = 8883;
char *pubTopic = "$aws/things/******/shadow/******/update/delta";
char *subTopic = "$aws/things/******/shadow/******/update";
  
const char* rootCA = "-----BEGIN CERTIFICATE-----\n" \
"**********\n"
"-----END CERTIFICATE-----\n";
  
const char* certificate = "-----BEGIN CERTIFICATE-----\n" \
"**********\n"
"-----END CERTIFICATE-----\n";
  
const char* privateKey = "-----BEGIN RSA PRIVATE KEY-----\n" \
"**********\n"
"-----END RSA PRIVATE KEY-----\n";
  
WiFiClientSecure httpsClient;
PubSubClient mqttClient(httpsClient);

//Servoオブジェクトを作成
Servo myservo;



void setup() {
    Serial.begin(115200);
  
    // Start WiFi
    Serial.println(F("次のWi-Fiに接続。"));
    Serial.print(ssid);
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(F("\n接続完了。"));
  
    // Configure MQTT Client
    httpsClient.setCACert(rootCA);
    httpsClient.setCertificate(certificate);
    httpsClient.setPrivateKey(privateKey);
    mqttClient.setServer(endpoint, port);
    mqttClient.setCallback(mqttCallback);
  
    connectAWSIoT();

    myservo.attach(13);
}
  
void connectAWSIoT() {
    while (!mqttClient.connected()) {
        if (mqttClient.connect("reo_smartkey")) {
            Serial.println(F("AWSIoTへの接続完了。"));
            int qos = 0;
            mqttClient.subscribe(subTopic, qos);
            Serial.println(F("subscribe完了"));
        } else {
            Serial.print(F("Failed. Error state="));
            Serial.print(mqttClient.state());
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
  
void mqttCallback (char* topic, byte* payload, unsigned int length) {
    Serial.print(F("Received. topic="));
    Serial.println(topic);
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.print("\n");

//subsclibeされたjsonを読み取り
    StaticJsonDocument<64> doc;
    deserializeJson(doc, payload);
    const char* Order = doc["Order"];
    String order = String(Order);

//publishするjsonの組み立て
    StaticJsonDocument<64> key_status;
    char jsonBuffer[512];
    
    if (order == "Unlock"){
      myservo.write(92);
      key_status["Status"] = "Unlocked";
      serializeJson(key_status, jsonBuffer);
      mqttClient.publish(pubTopic, jsonBuffer);
      
      Serial.println(F("解錠完了メッセージ"));
      Serial.println(pubTopic);
      Serial.println(jsonBuffer);
      Serial.println(F("Publish完了。"));
    }
    if (order == "Lock"){
      myservo.write(3);
      key_status["Status"] = "Locked";
      serializeJson(key_status, jsonBuffer);
      mqttClient.publish(pubTopic, jsonBuffer);
      
      Serial.print(F("施錠完了メッセージ"));
      Serial.println(pubTopic);
      Serial.println(jsonBuffer);
      Serial.println(F("Publish完了"));
    }
    if (order == "Reboot"){
      key_status["Status"] = "Rebooted";
      serializeJson(key_status, jsonBuffer);
      mqttClient.publish(pubTopic, jsonBuffer);
      
      Serial.println(pubTopic);
      Serial.println(jsonBuffer);
      Serial.println(F("Publish完了"));      
      Serial.println(F("再起動"));
      ESP.restart();
    }
}
//publishするjsonの組み立ては以下を参照
//https://www.souichi.club/m5stack/m5stickc-aws-iot-core/

void mqttLoop() {
    if (!mqttClient.connected()) {
        connectAWSIoT();
    }
    mqttClient.loop();
    delay(100);
}
  
void loop() {
  mqttLoop();
}