//#include <ota.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <RCSwitch.h>
#include "settings.h"

#define MQTT_RECONNECT_RATE 5000

//#define SERIAL_VERBOSE

long lastReconnect = 0;
long now = 0;
long lastSensorUpdate = 0;

bool setupDone = false;
bool canDeepSleep = false;

float lastTemperature = 0.0f;
float lastHumidity = 0.0f;
bool triggerState = false;
String mqttListenTopic;
String mqttSensorsTopic;
String mqttAlertsTopic;

Settings settings;
WiFiClient espClient;
PubSubClient client;
DHT dht;
RCSwitch mySwitch = RCSwitch();

void callback(char *topic, byte *payload, unsigned int length);

void reconnect()
{


        if (WiFi.status() == WL_CONNECTED)
        {

                if (!setupDone)
                {
                        ArduinoOTA.setHostname((const char *)WiFi.hostname().c_str());
                        ArduinoOTA.begin();
                        client.setClient(espClient);
                        client.setCallback(callback);

                        setupDone = true;
                }

                client.setServer(settings.mqttServer(), 1883);

                Serial.print("Attempting MQTT connection... ");
                // Attempt to connect
                if (client.connect((const char *)WiFi.hostname().c_str()))
                {
                        Serial.println("connected");
                        // ... and (re)subscribe
                        client.subscribe(mqttListenTopic.c_str());
                }
                else
                {
                        Serial.print("failed, rc=");
                        Serial.print(client.state());
                        Serial.println(" try again in 5 seconds");
                }
        }
}

void callback(char *topic, byte *payload, unsigned int length)
{

        // String sTopic = topic;

        strtok(topic, "/"); // First is always hostname -> ignore it
        char *mainTopic = strtok(NULL, "/");
        char *netId = strtok(NULL, "/");
        char *deviceId = strtok(NULL, "/");

        if (strcmp("elro_tx", mainTopic) == 0)
        {
                if (netId && deviceId)
                {
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

                        if (value)
                        {
                                mySwitch.switchOn(netId, deviceId);
                        }
                        else
                        {
                                mySwitch.switchOff(netId, deviceId);
                        }
                }
        }
        if (strcmp("sleep", mainTopic) == 0)
        {
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

        if (!settings.readFromFlash())
        {
                Serial.println("Reading settings failed");
        }
        else
        {
                settings.printData();
        }

        mqttSensorsTopic = String(settings.hostname()) + "/sensors";
        mqttAlertsTopic = String(settings.hostname()) + "/alert";
        mqttListenTopic = String(settings.hostname()) + "/#";

        if (settings.useDht())
        {
                dht.setup(settings.dhtPin());
        }

        if (settings.useElroSender())
        {
                mySwitch.enableTransmit(settings.elroSenderPin());
                mySwitch.setRepeatTransmit(15);
        }

        if (settings.useTriggerInput())
        {
                pinMode(settings.triggerInputPin(), INPUT_PULLUP);
        }

        WiFi.hostname(settings.hostname());
        WiFi.mode(WIFI_STA);
        WiFi.begin(settings.ssid(), settings.psk());
        bool led = true;
        while (WiFi.status() == WL_IDLE_STATUS)
        {
                
                delay(500);
                digitalWrite(BUILTIN_LED, led);
                led = !led;
        }

        WiFi.printDiag(Serial);

        Serial.print("Connected, IP address: ");
        Serial.println(WiFi.localIP());

        Serial.println("Setup finished");

        if (settings.disableLed())
                digitalWrite(BUILTIN_LED, HIGH);
}

void loop()
{
        now = millis();
        ArduinoOTA.handle();
        //Serial.println("test");

        if (!client.connected())
        {
                if (!settings.disableLed())
                        digitalWrite(BUILTIN_LED, LOW);
                if ((now - lastReconnect) > MQTT_RECONNECT_RATE)
                {
                        reconnect();
                        lastReconnect = now;
                }
                delay(10);
        }
        else
        {
                if (!settings.disableLed())
                        digitalWrite(BUILTIN_LED, HIGH);
                client.loop();

                if (settings.useDht())
                {
                        float h = dht.getHumidity();
                        float t = dht.getTemperature();

                        if (!isnan(t) && !isnan(h))
                        {
                                if ((abs(lastTemperature - t) > 0.1) || (abs(lastHumidity - h) > 1.0))
                                {
                                        Serial.print("Temperature update: ");
                                        Serial.print(t);
                                        Serial.println("°C");

                                        Serial.print("Humidity update: ");
                                        Serial.print(h);
                                        Serial.println("%");

                                        String tempString(t, 2);
                                        String humString(h, 2);

                                        String msg = String("{\"humidity\": " + humString + ", \"temperature\": " + tempString + "}");

                                        client.publish(mqttSensorsTopic.c_str(), msg.c_str(), true);

                                        lastHumidity = h;
                                        lastTemperature = t;
                                }
                        }
                }
                if (settings.useTriggerInput())
                {
                        bool trigger = digitalRead(settings.triggerInputPin());
                        if(triggerState != trigger) {
                                String msg = String(triggerState); //PullUP
                                client.publish(mqttAlertsTopic.c_str(), msg.c_str());
                                triggerState = trigger;
                        }
                        
                }
                // Serial.println("Going to sleep");
                // ESP.deepSleep(10e6);
                delay(10);
        }
} // End loop()
