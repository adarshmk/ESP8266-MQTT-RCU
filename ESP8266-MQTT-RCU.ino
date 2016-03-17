#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define MQTT_SERVER   "192.168.0.118"
#define MQTT_PORT     1883
#define MY_SSID       "Kadubi Home "
#define MY_PASSWD     "8065350423"

IPAddress myIP;
WiFiClient wifiClient;
int status = WL_IDLE_STATUS;
PubSubClient mqttClient (MQTT_SERVER, MQTT_PORT, messageArrived, wifiClient);

void setup() {
  Serial.begin(115200);
  Serial.println("Begin");

  while ( status != WL_CONNECTED ) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(MY_SSID);
    status = WiFi.begin(MY_SSID, MY_PASSWD);
    delay(10000);
  }

  Serial.println("Successfully connected!");
  myIP = WiFi.localIP();
  Serial.println(myIP);
}

String DisplayIPAddress (IPAddress address)
{
  return String(address[0]) + "." +
         String(address[1]) + "." +
         String(address[2]) + "." +
         String(address[3]);
}

void debug (String string) {
  char buf[100];
  string.toCharArray(buf, string.length()+1);
  mqttClient.publish("arduino/debug", buf);
}

void mqttConnect() {
  while (!mqttClient.connected())
  {
    if (mqttClient.connect("arduinoClient"))
    {
      mqttClient.subscribe("arduino/test");
      debug("I'm Alive");
      debug("My IP is: " + DisplayIPAddress(WiFi.localIP()));
    }
  }
}

void loop() {
  // Create a client connection
  if (!mqttClient.connected()) {
    mqttConnect();
  }

  mqttClient.loop();
}

//handles message arrived on subscribed topic(s)
void messageArrived(char* topic, byte* payload, unsigned int length) {

  if (strcmp(topic, "arduino/test") == 0) {
    byte* p = (byte*)malloc(length);
    // Copy the payload to the new buffer
    memcpy(p,payload,length);
    String pubStr = String((char*)p);
    debug(pubStr);
    // Free the memory
    free(p);
  }
}
