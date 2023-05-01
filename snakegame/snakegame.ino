// Include the BNO08x IMU library for gyro data
#include <Adafruit_BNO08x.h>
// See https://github.com/ivanseidel/LinkedList
#include <LinkedList.h>
// Include the NeoPixel library
#include <Adafruit_NeoPixel.h>

// Define number of LEDs per face
#define LED_COUNT 36
// Define the Arduino pin for each face
#define FACE0_LED_PIN 0
#define FACE1_LED_PIN 1
#define FACE2_LED_PIN 2
#define FACE3_LED_PIN 3
#define FACE4_LED_PIN 4
#define FACE5_LED_PIN 5

#define RESET_BUTTON_PIN 7

// Declare each NeoPixel face object and an array of all the faces
Adafruit_NeoPixel face0(LED_COUNT, FACE0_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel face1(LED_COUNT, FACE1_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel face2(LED_COUNT, FACE2_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel face3(LED_COUNT, FACE3_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel face4(LED_COUNT, FACE4_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel face5(LED_COUNT, FACE5_LED_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel faces[6] = {face0, face1, face2, face3, face4, face5};

const int numFaces = sizeof(faces) / sizeof(faces[0]);

long snakeUpdateTime;
unsigned long previousTime;

// Declare variables for gyroscope data
#define BNO08X_RESET -1
Adafruit_BNO08x  bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;
int16_t gx, gy, gz;
int16_t prev_gx = 0;
int16_t prev_gy = 0;
int16_t prev_gz = 0;
// Threshold sensor value required to register a turn in the snake
const int16_t THRESHOLD = 3;

// Winning snake length
const int MAX_LENGTH = 20;

// Define pixel datatype
struct pixel
{
  int face;
  int id;
};

// Define snake datatype
struct snake_type
{
  // a linked list of pixels representing the body of the snake
  LinkedList<pixel> body = LinkedList<pixel>();
  // the snake's current direction
  char direction;   // 'u' = UP, 'd' = DOWN, 'l' = LEFT, 'r' = RIGHT
};

// Declare snake object
snake_type snake;
// Declare apple variable
pixel apple;

// Define color constants for snake and apple pixels
const uint32_t snakeColor = face0.Color(0, 255, 0);
const uint32_t appleColor = face0.Color(255, 0, 0);

void setup(void) {
  Serial.begin(115200);
  // Pause execution until serial console opens
  while (!Serial) delay (10);

  if (!bno08x.begin_I2C()) {
    Serial.println("Failed to find BNO08x chip");
    while (1) { delay(10); }
  }
  Serial.println("BNO08x found");
  setReports();

  pinMode(RESET_BUTTON_PIN, INPUT);

  reset_game();
}

// Define the sensor outputs that need to be received
void setReports(void) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED)) {
    Serial.println("Could not enable gyroscope");
  }
}

void loop () {
  delay(10);
  check_reset();
  get_direction();
  unsigned long currentTime = millis();
  if(currentTime - previousTime > snakeUpdateTime) {
    move_snake();
    pixel head = snake.body.get(0);
    check_collision(head);
    check_apple(head);
    check_win();
    previousTime = currentTime;
  }
}

void initialize_snake (void) {
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
  // Generate random pixel and add it to the snake body
  pixel p = {random(numFaces), random(LED_COUNT))};
  snake.body.add(p);
  // Turn on pixel p
  faces[p.face].clear();
  faces[p.face].setPixelColor(p.id, snakeColor);
  faces[p.face].show();
  // Initialize snake direction as up
  snake.direction = 'u';
}

// Spawn the apple at a random pixel that is not in the snake body
void spawn_apple (void) {
  int snake_size = snake.body.size();
  bool in_snake;
  do {
    apple = {random(numFaces), random(LED_COUNT)};
    in_snake = false;
    for (int i = 0; i < snake_size; i++) {
      pixel body = snake.body.get(i);
      if (apple.face == body.face && apple.id == body.id) {
        in_snake = true;
        Serial.println("apple in snake");
        break;
      }
    }    
  }
  while (in_snake);
  // Turn on the apple pixel
  faces[apple.face].setPixelColor(apple.id, appleColor);
  faces[apple.face].show();
}

// Determine snake direction based on gyro data
void get_direction (void) {
  if (bno08x.wasReset()) {
    Serial.println("Sensor was reset");
    setReports();
  }

  if (!bno08x.getSensorEvent(&sensorValue)) {
    Serial.println("Failed to read sensor value");
  }

  switch (sensorValue.sensorId) {
    case SH2_GYROSCOPE_CALIBRATED:
      gx = sensorValue.un.gyroscope.x;
      gy = sensorValue.un.gyroscope.y;
      gz = sensorValue.un.gyroscope.z;
      Serial.print("Gyro - x: "); Serial.print(gx);
      Serial.print(" y: "); Serial.print(gy);        
      Serial.print(" z: "); Serial.println(gz); 

      // We only want the maximum gyro input to determine the new direction
      int16_t max_g = max(abs(gx), max(abs(gy), abs(gz)));
      if (max_g <= THRESHOLD) {
        break;
      }      

      if (gy > THRESHOLD && prev_gy < THRESHOLD) {
        Serial.println("Above threshold");
      }
      prev_gx = gx;
      prev_gy = gy;
      prev_gz = gz;
      break;
  }
}

void move_snake(void) {
  // Update the new head pixel of the snake according to
  // the snake's current direction, and turn on the new head
}

void check_collision(pixel head) {
  // Check if the head of the snake is equal to one of its body pixels
  // If so, end the game with a loss
  int snake_size = snake.body.size();
  for (int i = 1; i < snake_size; i++) {
      pixel body = snake.body.get(i);
      if (head.face == body.face && head.id == body.id) {
        Serial.println("Game over.");
        while (!check_reset()) {
          delay(100);
        }
      }
  }
}

void check_apple(pixel head) {
  // If head of the snake is equal to apple pixel, turn off the apple and spawn a new apple
  // Else turn off the tail pixel of the snake
  if (head.face == apple.face && head.id == apple.id) {
    faces[apple.face].setPixelColor(apple.id, 0);
    spawn_apple();
  }
  else {
    pixel tail = snake.body.pop();
    faces[tail.face].setPixelColor(tail.id, 0);
    faces[tail.face].show();
  }
}

void check_win(void) {
  // If size of snake is max length, end the game with a win
  if (snake.body.size() == MAX_LENGTH) {
    Serial.println("You win.");
    while (!check_reset()) {
          delay(100);
        }
  }
}

bool check_reset(){
  if (digitalRead(RESET_BUTTON_PIN) == LOW){
    reset_game();
    return true;
  }
  return false;
}

void reset_game() {
  // Clear the snake
  snake.body.clear();
  // Read the value at the potentiometer pin
  int potValue = analogRead(A0);
  // Set the snake's speed according to the potentiometer's state
  // The potentiometer thus functions as a "speed dial"
  snakeUpdateTime = 100 + round(0.9 * potValue);

  // Set up the NeoPixel faces
  for (int i = 0; i < numFaces; i++){
    faces[i].begin();
    faces[i].clear();
    faces[i].setBrightness(20);
  }

  initialize_snake();
  spawn_apple();

  delay(100);
  previousTime = millis();
}