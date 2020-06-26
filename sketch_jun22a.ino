#include "PubSubClient.h"
#include "ArduinoJson.h"
#include "DHT.h"
#include "ESP8266WiFi.h"
#include "NTPClient.h"
#include "WiFiUdp.h"

DHT dht = DHT(2, DHT11, 6);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const char* ssid     = "Nokia 6.1 Plus";
const char* password = "";

const char* mqtt_server = "lion.rmq.cloudamqp.com"; 
const char* mqtt_user = "ylhnwvqf:ylhnwvqf";
const char* mqtt_pass= "U0gpsudyXQBu8LiwAIOc7um4Av9ajPBd";

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() 
{
  // Loop until we're reconnected
  Serial.println("In reconnect...");
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("test_exchange", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  
  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)");
  dht.begin();
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); 

  client.setServer(mqtt_server, 1883);
  
  timeClient.begin();
}

void loop()
{
  
  if (!client.connected()) 
  {
    reconnect();
  }
  
  float humidity = dht.readHumidity(); /* Get humidity value */
  float temperature = dht.readTemperature(); /* Get temperature value */
  timeClient.update();

  char output[300];

  StaticJsonDocument<300> doc;
  doc["sensor"] = "DHT11";
  doc["humidity"] = humidity;
  doc["temp"] = temperature;
  doc["timestamp"] = timeClient.getEpochTime();

  serializeJson(doc, output);
  Serial.println(output);

  client.publish("nodemcu_data_queue",  output);
  
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\n");
  
  delay(5000);
}
