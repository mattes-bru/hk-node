#include <arduino.h>


class Settings {

public:
  Settings();

  bool readFromFlash();
  bool writeToFlash();

  String hostname() const;
  bool useDht() const;
  int dhtPin() const;
  String mqttServer() const;
  bool useElroSender() const;
  int elroSenderPin() const;
  bool useTriggerInput() const;
  int triggerInputPin() const;
  String ssid() const;
  String psk() const;
  bool disableLed() const;


  void printData() const;


private:
  String m_hostname;
  bool m_useDht = false;
  int m_dhtPin = 0;
  String m_mqttServer;
  bool m_useElroSender = false;
  int m_elroSenderPin = 0;
  bool m_useTriggerInput = false;
  int m_triggerInputPin = 0;
  String m_ssid;
  String m_psk;
  bool m_disableLed = false;





};
