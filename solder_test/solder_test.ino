//Simple test to ensure soldered connections work
//Starts with id of 0 (first LED) and also resorts to id of 0 when an invalid input is given (id not in range of IDs)

#include <Adafruit_NeoPixel.h> // Include the NeoPixel library

#define LED_PIN 6 // define the Arduino pin that the first NeoPixel's DIN is connected to
#define LED_COUNT 36 // define the number of pixels

// Initialize the Neopixel object as "stick"
Adafruit_NeoPixel stick(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

byte incomingId = 0;

int ledId = 0;

void setup() {
  stick.begin();
  stick.show();
  stick.setBrightness(20);
}

void loop() {
  ledId++;
  if (ledId > LED_COUNT - 1){
    ledId = 0;
  }
  showSelected(ledId, stick.Color(0, 0, 255));
}

void showSelected(int id, uint32_t color){
  stick.clear();
  stick.setPixelColor(id, color);
  stick.show();
  delay(500);
}