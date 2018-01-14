#include "settings.h"
#include <FS.h>
#include <ArduinoJson.h>


#define HOSTNAME "hostname"
#define MQTT_SERVER "mqtt_server"
#define SSID "ssid"
#define PSK "psk"
#define DHT "dht"
#define ELRO_SENDER "elro_sender"

Settings::Settings()
{
        SPIFFS.begin();

}


bool Settings::readFromFlash()
{


        File f = SPIFFS.open("/config.json", "r");
        if(!f) {
                Serial.println("can't open settings file");
                return false;
        } else {
                DynamicJsonBuffer jsonBuffer;
                JsonObject& config = jsonBuffer.parseObject(f.readString());

                if(!config.success()) {
                  Serial.println("failed to parse json");
                  return false;
                } else {
                  m_hostname = config.get<String>(HOSTNAME);
                  m_mqttServer = config.get<String>(MQTT_SERVER);
                  m_ssid = config.get<String>(SSID);
                  m_psk = config.get<String>(PSK);

                  m_useDht = config.containsKey(DHT);
                  if(m_useDht) {
                    m_dhtPin = config.get<int>(DHT);
                  }
                  m_useElroSender = config.containsKey(ELRO_SENDER);
                  if(m_useElroSender) {
                    m_elroSenderPin = config.get<int>(ELRO_SENDER);
                  }

                  return true;

                }

        }


/*
        //HOSTNAME
        File f = SPIFFS.open("/conf/hostname.txt", "r");
        if(!f) {
                return false;
        } else {
                m_hostname = f.readStringUntil('\n');
        }
        f.close();


        f = SPIFFS.open("/conf/mqtt.txt", "r");
        if(!f) {
                return false;
        } else {
                m_mqttServer = f.readStringUntil('\n');
        }
        f.close();

        f = SPIFFS.open("/conf/ssid.txt", "r");
        if(!f) {
                return false;
        } else {
                m_ssid = f.readStringUntil('\n');
        }
        f.close();

        f = SPIFFS.open("/conf/psk.txt", "r");
        if(!f) {
                return false;
        } else {
                m_psk = f.readStringUntil('\n');
        }
        f.close();

        f = SPIFFS.open("/conf/dht.txt", "r");
        if(!f) {
                m_useDht = false;
        } else {
                m_dhtPin = f.parseInt();
                //TODO sanity check
                m_useDht = true;
        }
        f.close();

        f = SPIFFS.open("/conf/elro_send.txt", "r");
        if(!f) {
                m_useElroSender = false;
        } else {
                m_elroSenderPin = f.parseInt();
                //TODO sanity check
                m_useElroSender = true;
        }
        f.close();


        return true;

        */

}

void Settings::printData() const
{
        if(Serial) {
                Serial.println("Settings:");

                Serial.print("Hostname:\t");
                Serial.println(m_hostname);

                Serial.print("WIFI:\t");
                Serial.print(m_ssid);
                Serial.print(" ");
                Serial.println(m_psk);

                Serial.print("MQTT Server:\t");
                Serial.println(m_mqttServer);

                Serial.print("DHT:\t");
                Serial.print(m_useDht);
                Serial.print( " (");
                Serial.print(m_dhtPin);
                Serial.println( ")");

                Serial.print("ELRO Sender:\t");
                Serial.print(m_useElroSender);
                Serial.print( " (");
                Serial.print(m_elroSenderPin);
                Serial.println( ")");
        }
}


String Settings::hostname() const
{
        return m_hostname;
}

String Settings::ssid() const
{
  return m_ssid;
}

String Settings::psk() const
{
  return m_psk;
}

bool Settings::useDht() const
{
        return m_useDht;
}

int Settings::dhtPin() const
{
        return m_dhtPin;
}

String Settings::mqttServer() const
{
        return m_mqttServer;
}

bool Settings::useElroSender() const
{
        return m_useElroSender;
}

int Settings::elroSenderPin() const
{
        return m_elroSenderPin;
}
