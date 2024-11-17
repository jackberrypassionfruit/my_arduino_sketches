#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h> 

#define RGB_BUILTIN 48
#define NUMPIXELS 16 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, RGB_BUILTIN, NEO_GRB + NEO_KHZ800);

const char* ssid = "raspi-webgui";
const char* password = "ChangeMe";
const char* mqtt_server = "192.168.0.211";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
int loop_iter = 0;
bool state_toggle;

void setup() {
  delay(100);
  Serial.begin(115200);
  while (!Serial) {delay(100);};

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  StaticJsonDocument<80> doc;
  char output[80];

  long now = millis();
  long interval_delta = now - lastMsg;

  if (interval_delta > 2500) {
    set_rgb_led_state(1);
  } else {
    set_rgb_led_state(0);
  }

  if (interval_delta > 5000) {
    lastMsg = now;

    // put your main code here, to run repeatedly:
    Serial.print("loop_iter: ");
    Serial.println(loop_iter);
    
    doc["loop_iter"] = loop_iter;
    doc["millis_timestamp"] = now;

    serializeJson(doc, output);
    Serial.println(output);
    client.publish("/home/sensors", output);

    ++loop_iter;  
  }
  delay(100);  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void set_rgb_led_state(bool state) {
  if (state) {
    pixels.fill(0x111111);
  } else {
    pixels.fill(0x000000);
  }
  pixels.show();
}