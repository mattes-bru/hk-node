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


#define HOSTNAME "HK-bedroom-"

#define SENSOR_UPDATE_RATE 1000
#define MQTT_RECONNECT_RATE 5000

// #define LED_GREEN D2
// #define LED_BLUE D3
// #define LED_RED  D4

//#define SERIAL_VERBOSE

const char* mqtt_server = "homebridge-gateway";
long lastReconnect = 0;
long now = 0;
long lastSensorUpdate = 0;

bool setupDone = false;

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(D1,DHT22);
RCSwitch mySwitch = RCSwitch();


void callback(char* topic, byte* payload, unsigned int length) ;



void reconnect() {

  if(WiFi.status() == WL_CONNECTED) {

    if(!setupDone) {
      ArduinoOTA.setHostname((const char *)WiFi.hostname().c_str());
      ArduinoOTA.begin();
      client.setServer(mqtt_server, 1883);
      client.setCallback(callback);

      setupDone = true;
    }

    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect((const char *)WiFi.hostname().c_str())) {
      Serial.println("connected");

      // ... and resubscribe
      // client.subscribe("fairylight");
      client.subscribe("elro2/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }

  }

}


void callback(char* topic, byte* payload, unsigned int length) {

  // String sTopic = topic;

  char *mainTopic = strtok(topic, "/");
  char *netId = strtok(NULL, "/");
  char *deviceId = strtok(NULL, "/");

  if(strcmp("elro2", mainTopic) == 0) {
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


}



void setup()
{
  Serial.begin(9600);
  //Internal LED
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  delay(100);

  Serial.println("Setup started");

  // pinMode(LED_RED,OUTPUT);
  // pinMode(LED_GREEN,OUTPUT);
  // pinMode(LED_BLUE,OUTPUT);
  //
  // analogWrite(LED_RED,0);
  // analogWrite(LED_GREEN,0);
  // analogWrite(LED_BLUE,0);

  dht.begin();
  mySwitch.enableTransmit(D0);
  mySwitch.setRepeatTransmit(15);



  //setupOTA();



  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wpa_key);

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
  }
  else {
    digitalWrite(BUILTIN_LED, HIGH);
    client.loop();

    if(now - lastSensorUpdate > SENSOR_UPDATE_RATE) {


      float h = dht.readHumidity();
      float t = dht.readTemperature();
      float light = analogRead(A0)/10.24;

      if(!isnan(h)) {
        String humString(h,2);
        client.publish("bedroom/sensors/hum", humString.c_str() );

      }
      if(!isnan(t)) {
        String tempString(t,2);
        client.publish("bedroom/sensors/temp", tempString.c_str() );

      }
      lastSensorUpdate = now;

    }



  }
  delay(10);



}
