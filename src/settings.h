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
  String ssid() const;
  String psk() const;


  void printData() const;


private:
  String m_hostname;
  bool m_useDht = false;
  int m_dhtPin = 0;
  String m_mqttServer;
  bool m_useElroSender = false;
  int m_elroSenderPin = 0;
  String m_ssid;
  String m_psk;





};
