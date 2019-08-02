#include <arduino.h>


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
  const char* m_hostname;
  bool m_useDht = false;
  int m_dhtPin = 0;
  const char* m_mqttServer;
  bool m_useElroSender = false;
  int m_elroSenderPin = 0;
  bool m_useTriggerInput = false;
  int m_triggerInputPin = 0;
  const char* m_ssid;
  const char* m_psk;
  bool m_disableLed = false;





};
