#include "config.h"
#include "secrets.h"

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht;
unsigned long previous_miliseconds = 0;
unsigned long present_miliseconds = 0;

float temperatura;
float wilgotnosc;

void wifi_connect(){
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(600);
  wifiManager.setAPStaticIPConfig(IPAddress(AP_IP_OCTET_1,AP_IP_OCTET_2,AP_IP_OCTET_3,AP_IP_OCTET_4), IPAddress(AP_GATEWAY_OCTET_1,AP_GATEWAY_OCTET_2,AP_GATEWAY_OCTET_3,AP_GATEWAY_OCTET_4), IPAddress(AP_SUBNET_OCTET_1,AP_SUBNET_OCTET_2,AP_SUBNET_OCTET_3,AP_SUBNET_OCTET_4));
  wifiManager.autoConnect();
}

void mqtt_connect(){
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect(MQTT_CLIENT_NAME, MQTT_USERNAME, MQTT_PASSWORD, MQTT_WILL_TOPIC, 0, true, MQTT_WILL_MESSAGE)) {
      client.publish(MQTT_CONFIG_TOPIC, MQTT_CONFIG_MESSAGE);
      client.publish(MQTT_CONFIG_TOPIC_2, MQTT_CONFIG_MESSAGE_2);
      client.publish(MQTT_STATUS_TOPIC, MQTT_STATUS_MESSAGE_ON);
      Serial.println("connected"); 
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
}

void send_data(){
  temperatura = dht.getTemperature();
  wilgotnosc = dht.getHumidity();
  while(dht.getStatus() != DHT::DHT_ERROR_t::ERROR_NONE){
    delay(dht.getMinimumSamplingPeriod());
    temperatura = dht.getTemperature();
    wilgotnosc = dht.getHumidity();
  }
  String payload = "";
  payload = payload + "{ \"temperature\": " + temperatura + ", \"humidity\": " + wilgotnosc + " }";
  client.publish(MQTT_DATA_TOPIC, payload.c_str());
  Serial.println(payload);
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(DHT_START_DELAY);
  pinMode(AP_ENABLE_GPIO, INPUT);
  pinMode(DHT_DATA_GPIO, INPUT);
  dht.setup(DHT_DATA_GPIO, DHT::DHT_MODEL_t::DHT11);
  wifi_connect();
  Serial.println(WiFi.localIP());
  client.setServer(IPAddress(MQTT_IP_OCTET_1, MQTT_IP_OCTET_2, MQTT_IP_OCTET_3, MQTT_IP_OCTET_4), MQTT_PORT);
  client.setCallback(callback);
  while(client.connected() != true){
    mqtt_connect();
    delay(MQTT_SERVER_TRY_CONNECT_DELAY);
  }
}

void loop() {
  if(digitalRead(AP_ENABLE_GPIO) == LOW){
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(300);
    wifiManager.setAPStaticIPConfig(IPAddress(AP_IP_OCTET_1,AP_IP_OCTET_2,AP_IP_OCTET_3,AP_IP_OCTET_4), IPAddress(AP_GATEWAY_OCTET_1,AP_GATEWAY_OCTET_2,AP_GATEWAY_OCTET_3,AP_GATEWAY_OCTET_4), IPAddress(AP_SUBNET_OCTET_1,AP_SUBNET_OCTET_2,AP_SUBNET_OCTET_3,AP_SUBNET_OCTET_4));
    wifiManager.startConfigPortal();
  }
  while(WiFi.status() != WL_CONNECTED){
    wifi_connect();
  }
  while(client.connected() != true){
    if(WiFi.status() != WL_CONNECTED) return;
    mqtt_connect();
    delay(MQTT_SERVER_TRY_CONNECT_DELAY);
  }
  if(WiFi.status() != WL_CONNECTED || client.connected() != true) return;
  
  client.loop();
  present_miliseconds = millis();
  if(present_miliseconds - previous_miliseconds >= DHT_MEASUREMENT_TIME) {
    send_data();
    previous_miliseconds = present_miliseconds;
  }
}