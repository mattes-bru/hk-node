# Firmware for an ESP based home automation node


## Configuration

The node is configured by an json file named ```config.json```stored in root of the spiffs.

```json
{
  "hostname" : "hostname_for_this_node",
  "mqtt_server": "<mqtt_server_or_ip",
  "ssid": "wifi_ssid",
  "psk": "wifi_secret",
  "dht": 5,
  "elro_sender": 4
}
```
