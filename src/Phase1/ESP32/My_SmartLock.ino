#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ssl_client.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <Servo.h>
  
char *ssid = "GIGA-002-2.4G";
char *password = "91168169";
  
const char *endpoint = "an17yqxd5v3ts-ats.iot.ap-northeast-1.amazonaws.com";

const int port = 8883;
char *pubTopic = "$aws/things/reo_smartkey/shadow/key_order/update/delta";
char *subTopic = "$aws/things/reo_smartkey/shadow/key_order/update";
  
const char* rootCA = "-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
"rqXRfboQnoZsG4q5WTP468SQvvG5\n"

"-----END CERTIFICATE-----\n";
  
const char* certificate = "-----BEGIN CERTIFICATE-----\n" \
"MIIDWTCCAkGgAwIBAgIUQF7MMBOXYu3RyQp5r1XQX/is1agwDQYJKoZIhvcNAQEL\n"
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMDcyOTE0NTc0\n"
"M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMRagineMMNWIxYRJfy5\n"
"hRZlmcq4WYC4kY2xndFbbj5Jr9AESfOMXnzI5cmm/WV9G6bFBn4ts96i4RqitIRN\n"
"Q2WkVzXqFU0lGYhm/4Zdh4RLvkHTX7iwxMMG6i1AGsKMNC6HyYEV2TVEsQVkGoxg\n"
"I61bpeZ+TJmOSwGDVf/D5FRKs/m52ywwd1ZCz8YighY557U9pdxyYiL2SjOdeIOU\n"
"iJp4/735NSK8yqJxjxgQmsSUiro7URVx3cS2A6mh6Xp4++WRmHkGlV4WdZPCwgbl\n"
"ThP4WJwexwk3dNHrTp0g2RDOWawv/KK1OPganBrhJqX6Hj6O5kgFIwLvnZ2yXrH5\n"
"BaECAwEAAaNgMF4wHwYDVR0jBBgwFoAUkFa9C/dnmUGJvUtw/E5jypBmPiIwHQYD\n"
"VR0OBBYEFJVHwN9fGGFFeD3fOiTlTXLwUuKHMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQADdREKa+jr+4W0jnuyCFdPRj1O\n"
"Q6gOsA+Y1T6qulNjaIl5slLvCaJeiJzSr8RjI/Y9osbCuXvXIUPpf2TmblK5b+PO\n"
"SOijY8Zf1/cg97fE5j9TD23M6xlAFj/HeUi1eAZyrR9OaB7/Z7miFdVpljaq+LJc\n"
"mTeAb/cOo0R78hiGK3rRmYF7Agtd0hdTRiFlYXJGX37fLu+OUtKn/j4GMVMqdHbw\n"
"aprUHptZaOqkqfJaISFWAVZj2lL4rAyuCdggGLFLgqr4w6dWZnSm5LmzCIMQR5x8\n"
"ocL8g9zjJIx1ZVfE1Z8h+XGl6J6jPgfRnpm00khS+mQvHi3yNc0EJwj8/NCj\n"
"-----END CERTIFICATE-----\n";
  
const char* privateKey = "-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpQIBAAKCAQEAxFqCKd4ww1YjFhEl/LmFFmWZyrhZgLiRjbGd0VtuPkmv0ARJ\n"
"84xefMjlyab9ZX0bpsUGfi2z3qLhGqK0hE1DZaRXNeoVTSUZiGb/hl2HhEu+QdNf\n"
"uLDEwwbqLUAawow0LofJgRXZNUSxBWQajGAjrVul5n5MmY5LAYNV/8PkVEqz+bnb\n"
"LDB3VkLPxiKCFjnntT2l3HJiIvZKM514g5SImnj/vfk1IrzKonGPGBCaxJSKujtR\n"
"FXHdxLYDqaHpenj75ZGYeQaVXhZ1k8LCBuVOE/hYnB7HCTd00etOnSDZEM5ZrC/8\n"
"orU4+BqcGuEmpfoePo7mSAUjAu+dnbJesfkFoQIDAQABAoIBAQCZ8O+9zXJjowOt\n"
"CFLE+doqvgs+ERwyvAF5mnhFrC4ZbYIGT2Z/KFzn7kWhvGdMCzMDo9K8WC597SsY\n"
"y9lFJKIjbR3mqhh7PAvxkfbhrDBT+2I1j3ergAMzeBde1LRmNK+wj0rfBncduTYk\n"
"KOBbkABIqOK/e+OoPBP4IrTo6fb4UZ2MvsG07lZvKMKDakejaVN5NGRJ0HPVbuRL\n"
"oL1/xDYptw9+W7JpqBJDB9AN5Cdww048uGpPF8mv307eIa/MWJrMBkFzyETdd7ZE\n"
"YHVvasyHgbBlg2GGR2WTGnO7jvYzXxzGC10i6ZjsPC7vn6++CL4HTZX89BGTbTHe\n"
"n0F65WAlAoGBAOClSYZtcYM0UcK3bQAN3SJc1Owsw7YTgNftJ/ty2t3SJBTjSLzF\n"
"UawQLSoSwuc8H4lbLRp5BVGEpPfmAU51/Db/twIMAGzfqZ8ldOlik/r/JfPzYIUR\n"
"hKKE4FS/cm5L/WktyqHUuO8cERImbQTsGMxQcqfqsMWgWuLKb7wThHarAoGBAN/C\n"
"VCsYCj7ixiEe40pzYRYRPIJZ0TEcUJApd1x861JqUWTQ5eMyC1p20FgtdysFYv0y\n"
"KTUu/EQRL/U92Pd154fva3aLG+2QQRVoZXa1bQ/OwOK1IWl28vXh9hc3lcPyZ0g/\n"
"avlT7aatXyfF4AxklZKPuI8+9izUSkbJT1Hje2TjAoGAGIMQjcF9ZFKeJT/8fcgF\n"
"fA1jEk4kIMbqBSWUekRjC1c1pq3SLv7c1YiCVRfs+mw+CK/RMRVJdkah9Ya8V8JZ\n"
"/epTHr8OgNK64Gmm4xxlhzq1+nNw8bQio9UZ4UVuiAh0ozhi5tuePkDyv+c/eA0G\n"
"+6w/hoD+PuIraJmsLXp3trcCgYEApCA/aqikg0IFwC+O2oJNGUmlT5ugBNzW/xdb\n"
"VPzQA1C2yTMtHHwNtdpmsfcPbP/zfvL3JOqE4UyWF9W4urLCFIR6F6ar4pIoGxLL\n"
"Mcqb57do5iGkE17Lka2fu6Hq+aJpPaY+yglUzShoHp1W3OA+khVXc8LkATfqD4gD\n"
"5XXtga8CgYEAnf0bb7w99/r4J13xSoSh1W+/lCcjG5+T15esuRIB89VvEqewjUrj\n"
"U5+BD2/bdtHVpPWXuz6dmgD8O9xuMfHgRZIu+gOuNKQZQBxzkRFgZocL6gZWqkXX\n"
"TnMDEcELiCZXE5EJ7FTBUWY7c9A2j9yL3owB9/oLITq1WfHgSTqQJcU=\n"

"-----END RSA PRIVATE KEY-----\n";
  
WiFiClientSecure httpsClient;
PubSubClient mqttClient(httpsClient);

//Servoオブジェクトを作成
Servo myservo;

void setup() {
    Serial.begin(115200);
  
    // Start WiFi
    Serial.println("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected.");
  
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
            Serial.println("Connected.");
            int qos = 0;
            mqttClient.subscribe(subTopic, qos);
            Serial.println("Subscribed.");
        } else {
            Serial.print("Failed. Error state=");
            Serial.print(mqttClient.state());
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

char pubMessage[128];

void mqttCallback (char* topic, byte* payload, unsigned int length) {
    Serial.print("Received. topic=");
    Serial.println(topic);
 for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.print("\n");
    
    StaticJsonDocument<64> doc;
    deserializeJson(doc, payload);
    const char* Switch = doc["Switch"];
    String order = String(Switch);
    if (order == "Unlock"){
      myservo.write(100);
      sprintf(pubMessage, "{\"message\": \"Unlocked\"}");
      Serial.print("Publishing message to topic ");
      Serial.println(pubTopic);
      Serial.println(pubMessage);
      mqttClient.publish(pubTopic, pubMessage);
      Serial.println("Published.");
    }
    if (order == "Lock"){
      myservo.write(12);
      sprintf(pubMessage, "{\"message\": \"Locked\"}");
      Serial.print("Publishing message to topic ");
      Serial.println(pubTopic);
      Serial.println(pubMessage);
      mqttClient.publish(pubTopic, pubMessage);
      Serial.println("Published.");
    }
}

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