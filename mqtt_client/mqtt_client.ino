#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
// #include <Adafruit_Sensor.h>
// #include "Adafruit_BME680.h"
#include <Adafruit_NeoPixel.h> 

#define LIGHT_SENSOR_GPIO 11 
#define RGB_BUILTIN 48
#define NUMPIXELS 16 // Popular NeoPixel ring size

// Adafruit_BME680 bme;
Adafruit_NeoPixel pixels(NUMPIXELS, RGB_BUILTIN, NEO_GRB + NEO_KHZ800);

const char* ssid = "TP-Link_1936";
const char* password = "97792710";
const char* mqtt_server = "192.168.0.193";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
bool lastLightSensorBool = 0;


void setup() {
  delay(100);
  Serial.begin(115200);
  while (!Serial);

  // if (!bme.begin()) {
  //   Serial.println("Could not find a valid BME680 sensor, check wiring!");
  //   while (1);
  // }

  // Set up oversampling and filter initialization
  // bme.setTemperatureOversampling(BME680_OS_8X);
  // bme.setHumidityOversampling(BME680_OS_2X);
  // bme.setPressureOversampling(BME680_OS_4X);
  // bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  // bme.setGasHeater(320, 150); // 320*C for 150 ms
  pinMode(LIGHT_SENSOR_GPIO, OUTPUT);

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
  if (interval_delta > 5000) {
    if (interval_delta > 2500) {
      digitalWrite(RGB_BUILTIN, HIGH);   // Turn the RGB LED white
    } else {
      digitalWrite(RGB_BUILTIN, LOW);   // Turn the RGB LED white
    }
    lastMsg = now;
    // // float temp = bme.readTemperature();
    // // float pressure = bme.readPressure() / 100.0;
    // // float humidity = bme.readHumidity();
    // // float gas = bme.readGas() / 1000.0;
    lastLightSensorBool = digitalRead(LIGHT_SENSOR_GPIO);

    set_rgb_led_state(lastLightSensorBool);

    // doc["t"] = temp;
    // doc["p"] = pressure;
    // doc["h"] = humidity;
    // doc["g"] = gas;
    doc["t"] = lastLightSensorBool;

    serializeJson(doc, output);
    Serial.println(output);
    client.publish("/home/sensors", output);
  }
    
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
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