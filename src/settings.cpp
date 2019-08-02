#include "settings.h"
#include <FS.h>
#include <ArduinoJson.h>


#define HOSTNAME "hostname"
#define MQTT_SERVER "mqtt_server"
#define SSID "ssid"
#define PSK "psk"
#define DHT "dht"
#define ELRO_SENDER "elro_sender"
#define TRIGGER_INPUT "trigger_input"
#define DISABLE_LED "disable_led"

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
    } 

    StaticJsonDocument<400> config;
        
    DeserializationError error = deserializeJson(config, f.readString());
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return false;
    }

    m_hostname = config[HOSTNAME];
    m_mqttServer = config[MQTT_SERVER];
    m_ssid = config[SSID];
    m_psk = config[PSK];

    m_useDht = config.containsKey(DHT);
    if(m_useDht) {
        m_dhtPin = config[DHT];
        }
        m_useElroSender = config.containsKey(ELRO_SENDER);
        if(m_useElroSender) {
        m_elroSenderPin = config[ELRO_SENDER];
        }
        m_useTriggerInput = config.containsKey(TRIGGER_INPUT);
        if(m_useTriggerInput) {
        m_triggerInputPin = config[TRIGGER_INPUT];
        }
        m_disableLed = config[DISABLE_LED];

        return true;
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

        Serial.print("Trigger:\t");
        Serial.print(m_useTriggerInput);
        Serial.print( " (");
        Serial.print(m_triggerInputPin);
        Serial.println( ")");

        Serial.print("ELRO Sender:\t");
        Serial.print(m_useElroSender);
        Serial.print( " (");
        Serial.print(m_elroSenderPin);
        Serial.println( ")");
    }
}


const char* Settings::hostname() const
{
    return m_hostname;
}

const char* Settings::ssid() const
{
    return m_ssid;
}

const char* Settings::psk() const
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

const char* Settings::mqttServer() const
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

bool Settings::useTriggerInput() const
{
    return m_useTriggerInput;
}

int Settings::triggerInputPin() const
{
    return m_triggerInputPin;
}

bool Settings::disableLed() const
{
    return m_disableLed;
}
