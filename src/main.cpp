//#include <ota.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include "wifi_config.h"
#include <PubSubClient.h>
#include <DHT.h>
#include <RCSwitch.h>

#include "settings.h"


#define MQTT_RECONNECT_RATE 5000

// #define LED_GREEN D2
// #define LED_BLUE D3
// #define LED_RED  D4

//#define SERIAL_VERBOSE

long lastReconnect = 0;
long now = 0;
long lastSensorUpdate = 0;

bool setupDone = false;
bool canDeepSleep = false;

float lastTemperature = 0.0f;
float lastHumidity = 0.0f;

String mqttHumidityTopic;
String mqttTemperatureTopic;
String mqttListenTopic;

Settings settings;
WiFiClient espClient;
PubSubClient client;
DHT dht;
RCSwitch mySwitch = RCSwitch();


void callback(char* topic, byte* payload, unsigned int length);



void reconnect() {

        char mqtt_server[20];
        if(WiFi.status() == WL_CONNECTED) {

                if(!setupDone) {
                        settings.mqttServer().toCharArray(mqtt_server, 20);
                        ArduinoOTA.setHostname((const char *)WiFi.hostname().c_str());
                        ArduinoOTA.begin();
                        client.setClient(espClient);
                        client.setServer(mqtt_server, 1883);
                        client.setCallback(callback);

                        setupDone = true;
                }

                Serial.print("Attempting MQTT connection ");
                Serial.print(mqtt_server);
                Serial.print(" ...");
                // Attempt to connect
                if (client.connect((const char *)WiFi.hostname().c_str())) {
                        Serial.println("connected");
                        // ... and (re)subscribe
                        client.subscribe(mqttListenTopic.c_str());
                } else {
                        Serial.print("failed, rc=");
                        Serial.print(client.state());
                        Serial.println(" try again in 5 seconds");
                }

        }

}


void callback(char* topic, byte* payload, unsigned int length) {

        // String sTopic = topic;

        strtok(topic, "/"); // First is always hostname -> ignore it
        char *mainTopic = strtok(NULL, "/");
        char *netId = strtok(NULL, "/");
        char *deviceId = strtok(NULL, "/");

        if(strcmp("elro_tx", mainTopic) == 0) {
                if(netId && deviceId) {
                        bool value = (length == 4);

                        Serial.print("NET: ");
                        Serial.print(netId);
                        Serial.print("\tDEVICE: ");
                        Serial.print(deviceId);
                        Serial.print("\tVALUE: ");
                        Serial.print(value);
                        Serial.print(" / ");
                        Serial.print((char *)payload);
                        Serial.print(" (");
                        Serial.print(length);
                        Serial.println(")");

                        if(value) {
                                mySwitch.switchOn(netId, deviceId);
                        }
                        else {
                                mySwitch.switchOff(netId, deviceId);
                        }
                }
        }
        if(strcmp("sleep", mainTopic) == 0) {
          bool value = (length == 4);
          Serial.print("Sleep mode set to: ");
          Serial.println(value);
          canDeepSleep = value;
        }


}



void setup()
{
        Serial.begin(115200);
        //Internal LED
        pinMode(BUILTIN_LED, OUTPUT);
        digitalWrite(BUILTIN_LED, LOW);

        delay(100);

        Serial.println("Setup started");

        if(!settings.readFromFlash()) {
                Serial.println("Reading settings failed");
        }
        else {
                settings.printData();
        }

        mqttHumidityTopic = settings.hostname() + "/sensors/hum";
        mqttTemperatureTopic = settings.hostname() + "/sensors/temp";
        mqttListenTopic = settings.hostname() + "/#";

        // pinMode(LED_RED,OUTPUT);
        // pinMode(LED_GREEN,OUTPUT);
        // pinMode(LED_BLUE,OUTPUT);
        //
        // analogWrite(LED_RED,0);
        // analogWrite(LED_GREEN,0);
        // analogWrite(LED_BLUE,0);

        if(settings.useDht()) {
            dht.setup(settings.dhtPin());
        }

        if(settings.useElroSender()) {
          mySwitch.enableTransmit(settings.elroSenderPin());
          mySwitch.setRepeatTransmit(15);
        }


        WiFi.hostname(settings.hostname());
        WiFi.mode(WIFI_STA);
        WiFi.begin(settings.ssid().c_str(), settings.psk().c_str());

        Serial.println("Setup finished");


}

void loop()
{
        now = millis();
        ArduinoOTA.handle();
        //Serial.println("test");


        if (!client.connected()) {
                digitalWrite(BUILTIN_LED, LOW);
                if((now -lastReconnect) > MQTT_RECONNECT_RATE) {
                        reconnect();
                        lastReconnect = now;
                }
                delay(10);
        }
        else {
                digitalWrite(BUILTIN_LED, HIGH);
                client.loop();

                if(settings.useDht()) {
                  float h = dht.getHumidity();
                  float t = dht.getTemperature();

                  if(!isnan(t) && !isnan(h)) {
                          if(abs(lastTemperature - t)  >  0.1) {
                                  lastTemperature = t;
                                  String tempString(t,2);
                                  client.publish( mqttTemperatureTopic.c_str(), tempString.c_str(), true );
                          }

                          if(abs(lastHumidity - h)  >  1.0) {
                                  lastHumidity = h;
                                  String humString(h,2);
                                  client.publish(mqttHumidityTopic.c_str(), humString.c_str(), true );
                          }

                  }

                }

                delay(10);

        }






}
