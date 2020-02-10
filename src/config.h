#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <DHT.h>

#define DHT_DATA_GPIO 5
#define DHT_TYPE DHT11
#define DHT_MEASUREMENT_TIME 60000
#define DHT_START_DELAY 1000

#define MQTT_CLIENT_NAME "ESP "
#define MQTT_TOPIC "esp"
#define MQTT_CONFIG_TOPIC "homeassistant/sensor/espT/config"
#define MQTT_CONFIG_MESSAGE "{ \"device_class\": \"temperature\", \"name\": \"Temperature\", \"state_topic\": \"homeassistant/sensor/esp/data\", \"availability_topic\": \"homeassistant/sensor/esp/status\", \"payload_available\": \"on\", \"payload_not_available\": \"off\", \"unit_of_measurement\": \"°C\", \"value_template\": \"{{ value_json.temperature}}\" }"
#define MQTT_CONFIG_TOPIC_2 "homeassistant/sensor/espH/config"
#define MQTT_CONFIG_MESSAGE_2 "{ \"device_class\": \"humidity\", \"name\": \"Humidity\", \"state_topic\": \"homeassistant/sensor/esp/data\", \"availability_topic\": \"homeassistant/sensor/esp/status\", \"payload_available\": \"on\", \"payload_not_available\": \"off\", \"unit_of_measurement\": \"%\", \"value_template\": \"{{ value_json.humidity}}\" }"
#define MQTT_STATUS_TOPIC "homeassistant/sensor/esp/status"
#define MQTT_STATUS_MESSAGE_ON "on"
#define MQTT_STATUS_MESSAGE_OFF "off"
#define MQTT_DATA_TOPIC "homeassistant/sensor/esp/data"
#define MQTT_WILL_TOPIC MQTT_STATUS_TOPIC
#define MQTT_WILL_MESSAGE MQTT_STATUS_MESSAGE_OFF
#define MQTT_SERVER_TRY_CONNECT_DELAY 2000

#define AP_IP_OCTET_1 192       //Pierwszy oktet adresu IP
#define AP_IP_OCTET_2 168       //Drugi oktet adresu IP
#define AP_IP_OCTET_3 0         //Trzeci oktet adresu IP
#define AP_IP_OCTET_4 1         //Czwarty oktet adresu IP
#define AP_GATEWAY_OCTET_1 0    //Pierwszy oktet adresu bramy
#define AP_GATEWAY_OCTET_2 0    //Drugi oktet adresu bramy
#define AP_GATEWAY_OCTET_3 0    //Trzeci oktet adresu bramy
#define AP_GATEWAY_OCTET_4 0    //Czwarty oktet adresu bramy
#define AP_SUBNET_OCTET_1 255   //Pierwszy oktet adresu podsieci
#define AP_SUBNET_OCTET_2 255   //Drugi oktet adresu podsieci
#define AP_SUBNET_OCTET_3 255   //Trzeci oktet adresu podsieci
#define AP_SUBNET_OCTET_4 0     //Czwarty oktet adresu podsieci
#define AP_ENABLE_GPIO 4