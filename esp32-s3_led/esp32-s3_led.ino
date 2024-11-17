
#include <Adafruit_NeoPixel.h> 

#define LIGHT_SENSOR_GPIO 11
#define RGB_BUILTIN 48
#define NUMPIXELS 16 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, RGB_BUILTIN, NEO_GRB + NEO_KHZ800);

int loop_iter = 0;
bool state_toggle = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {delay(100);}
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pinMode(LIGHT_SENSOR_GPIO, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("loop_iter: ");
  Serial.println(loop_iter);

  // state_toggle = loop_iter % 2 == 0;
  state_toggle = digitalRead(LIGHT_SENSOR_GPIO);

  set_rgb_led_state(state_toggle);
  Serial.print("state_toggle: ");
  Serial.println(state_toggle);

  ++loop_iter;  
  delay(500); 
}

void set_rgb_led_state(bool state) {
  if (state) {
    pixels.fill(0x111111);
  } else {
    pixels.fill(0x000000);
  }
  pixels.show();
}
