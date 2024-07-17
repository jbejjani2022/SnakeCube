#include <Adafruit_NeoPixel.h> // Include the NeoPixel library

#define LED_COUNT 12
#define FACE0_LED_PIN 6

Adafruit_NeoPixel face0(LED_COUNT, FACE0_LED_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel faces[1] = {face0};

int dir = 0;
int ledId = 0;

const int numFaces = sizeof(faces) / sizeof(faces[0]);

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < numFaces; i++){
    faces[i].begin();
    faces[i].show();
    faces[i].setBrightness(20);
  }
}

void loop() {
  if (Serial.available() > 0){
    dir = get_dir();
    clearSerial();
    if (dir != -1){
      ledId = get_id(dir);
    }
  }
  faces[0].clear();
  faces[0].setPixelColor(ledId, 0, 0, 255);
  faces[0].show();
  delay(500);
}

int get_dir(){
  int result;
  result = Serial.parseInt();
  return result;
}

int get_id(int direction){
  if (direction == 1){
    return ledId - 6;
  }
  if (direction == 3){
    return ledId + 6;
  }
  if (direction == 5){
    return ledId + 1;
  }
  if (direction == 2){
    return ledId - 1;
  }
}

void clearSerial(){
  while (Serial.available()){
      Serial.read();
  }
}