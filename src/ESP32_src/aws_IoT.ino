#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "ArduinoJson.h" 

#define ID "ESP32" 
#define OUT_TOPIC "$aws/things/[device]/shadow/update"
#define IN_TOPIC "$aws/things/[device]/shadow/update/delta"

#OUT_TOIC => Publishする先のトピック
#IN_TOIC => Publishする先のトピック

const char* ssid     = "[ssid]";
const char* password = "[password]";

const char* server = "[end point].amazonaws.com";

const char* json = "{\"state\":{\"reported\":{\"led\":\"off\"}}}";

StaticJsonDocument<1024> doc;
const int port = 8883;
const int LED_pin = 15;

const char* Root_CA = \
"-----BEGIN CERTIFICATE-----\n" \

"-----END CERTIFICATE-----\n";

const char* Client_cert = \
"-----BEGIN CERTIFICATE-----\n" \

"-----END CERTIFICATE-----\n";

const char* Client_private = \
"-----BEGIN RSA PRIVATE KEY-----\n" \

"-----END RSA PRIVATE KEY-----\n";

WiFiClientSecure client;
PubSubClient mqttClient(client);

#AWS IoTとの接続
void connectAWSIoT() {
    while (!mqttClient.connected()) {
        if (mqttClient.connect("ESP32")) {
            Serial.println("Connected.");
        } else {
            Serial.print("Failed. Error state=");
            Serial.print(mqttClient.state());
            //5秒待ち
            delay(5000);
        }
    }
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_pin, OUTPUT);

  delay(10);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  delay(1000);

  Serial.println(Root_CA);
  Serial.println(Client_cert);
  Serial.println(Client_private);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setCACert(Root_CA);
  client.setCertificate(Client_cert);
  client.setPrivateKey(Client_private);
  mqttClient.setServer(server, port);  
  mqttClient.setCallback(callback);

  connectAWSIoT();

  int qos = 0;
  //通信できるデータの最大サイズ
  Serial.println(MQTT_MAX_PACKET_SIZE);
  if(mqttClient.subscribe(IN_TOPIC, qos)){
    Serial.println("Subscribed.");
    Serial.println("Success!!");
  }

    deserializeJson(doc, json);
    JsonObject& obj = doc.as<JsonObject>();

   if(mqttClient.publish(OUT_TOPIC, json)){
    Serial.println("Published!!");
   }
}

#subscribeした時に呼び出すコールバック関数
void callback (char* topic, byte* payload, unsigned int length) {
    Serial.println("Received. topic=");
    Serial.println(topic);
    char subsc[length];
    #文字の出力
    for (int i = 0; i < length; i++) {
        subsc[i] = (char)payload[i];
        subsc[length]='\0';
        Serial.print(subsc);
    }
    Serial.print("\n");
    #Subscribeした際にLチカ
    digitalWrite(LED_pin, HIGH);
}

void mqttLoop() {
    #PubsubClientのmainLoop->データの取得を待っている
    mqttClient.loop();
    delay(100);
    digitalWrite(LED_pin, LOW);
    Serial.print(".");

}

void loop() {
  mqttLoop();
  delay(1000);
}
