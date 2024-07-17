//Test of daisy chained NEOPIXEL sticks. Enter ID of LED in serial monitor to display that LED
//Starts with id of 0 (first LED) and also resorts to id of 0 when an invalid input is given (id not in range of IDs)

#include <Adafruit_NeoPixel.h> // Include the NeoPixel library

#define LED_PIN 6 // define the Arduin pin that the first NeoPixel's DIN is connected to
#define LED_COUNT 16 // define the number of pixels

// Initialize the Neopixel object as "stick"
Adafruit_NeoPixel stick(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

byte numberToDisplay = 170;
byte incomingId = 0;

int ledId = 0;

void setup() {
  Serial.begin(9600);
  stick.begin();
  stick.show();
  stick.setBrightness(20);
}

void loop() {
  if (Serial.available() > 0){
    ledId = get_id();
    clearSerial();
  }
  showSelected(ledId, stick.Color(0, 0, 255));
}

int get_id(){
  int result;
  result = Serial.parseInt();
  if (result < 0 || result > LED_COUNT - 1){
    result = 0;    
  }
  return result;
}

void clearSerial(){
  while (Serial.available()){
      Serial.read();
  }
}

void showSelected(int id, uint32_t color){
  stick.clear();
  stick.setPixelColor(id, color);
  stick.show();
  delay(500);
}