#include <arduino.h>
#include <ArduinoJson.h>

class Settings {

public:
  Settings();

  bool readFromFlash();
  bool writeToFlash();

  const char* hostname() const;
  bool useDht() const;
  int dhtPin() const;
  const char* mqttServer() const;
  bool useElroSender() const;
  int elroSenderPin() const;
  bool useTriggerInput() const;
  int triggerInputPin() const;
  const char* ssid() const;
  const char* psk() const;
  bool disableLed() const;


  void printData() const;


private:
  StaticJsonDocument<400> m_config;
  bool m_error = false;
 
};
