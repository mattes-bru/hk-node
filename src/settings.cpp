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

    
        
    DeserializationError error = deserializeJson(m_config, f.readString());
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        m_error = true;
        return false;
    }

    return true;

}



const char* Settings::hostname() const
{
    if (m_error) 
        return nullptr;
    else
        return m_config[HOSTNAME];
        
}

const char* Settings::ssid() const
{
    if (m_error) 
        return nullptr;
    else
        return m_config[SSID];
}

const char* Settings::psk() const
{
    if (m_error) 
        return nullptr;
    else
        return m_config[PSK];
}

bool Settings::useDht() const
{
        if (m_error) 
        return false;
    else
        return m_config.containsKey(DHT);
}

int Settings::dhtPin() const
{
    if (m_error) 
        return -1;
    else
        return m_config[DHT];
}

const char* Settings::mqttServer() const
{
    if (m_error) 
        return nullptr;
    else
        return m_config[MQTT_SERVER];
}

bool Settings::useElroSender() const
{
        if (m_error) 
        return false;
    else
        return m_config.containsKey(ELRO_SENDER);
}

int Settings::elroSenderPin() const
{
    if (m_error) 
        return -1;
    else
        return m_config[ELRO_SENDER];
}

bool Settings::useTriggerInput() const
{
    if (m_error) 
        return false;
    else
        return m_config.containsKey(TRIGGER_INPUT);
}

int Settings::triggerInputPin() const
{
    if (m_error) 
        return -1;
    else
        return m_config[TRIGGER_INPUT];
}

bool Settings::disableLed() const
{
    if (m_error) 
        return false;
    else
        return m_config[DISABLE_LED];
}

void Settings::printData() const
{
    Serial.println("Used config:");
    serializeJson(m_config, Serial);
    Serial.println();
    
}
