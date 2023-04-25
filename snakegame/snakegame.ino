#include <Adafruit_BNO08x.h>
// See https://github.com/ivanseidel/LinkedList
#include <LinkedList.h>

#define BNO08X_RESET -1

// Declare variables for gyroscope data
Adafruit_BNO08x  bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;
int16_t gx, gy, gz;
int16_t prev_gx = 0;
int16_t prev_gy = 0;
int16_t prev_gz = 0;
// Threshold sensor value required to register a turn in the snake
const int16_t THRESHOLD = 5;

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

  initialize_snake();
  spawn_apple();

  delay(100);
}

// Define the sensor outputs that need to be received
void setReports(void) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED)) {
    Serial.println("Could not enable gyroscope");
  }
}

void loop () {
  delay(100);
  get_gyro();
  move_snake();
}

void initialize_snake (void) {
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
  // Spawn the new length 1 snake at a random pixel
  pixel p = {random(6), random(36)};
  snake.body.add(p);
  // TODO: Turn on pixel p

  // Initialize snake direction as up
  snake.direction = 'u';
}

// Spawn the apple at a random pixel that is not in the snake body
void spawn_apple (void) {
  int snake_size = snake.body.size();
  bool in_snake = false;
  do {
    pixel apple = {random(6), random(36)};
    in_snake = false;
    for (int h = 0; h < snake_size; h++) {
      pixel body = snake.body.get(h);
      if (apple.face == body.face && apple.id == body.id) {
        in_snake = true;
        Serial.println("apple in snake");
        break;
      }
    }    
  }
  while (in_snake);
  // Turn on the apple pixel
}

void get_gyro (void) {
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
  // Turn off the tail pixel of the snake
  // Update the new head pixel of the snake according to
  // the snake's current direction, and turn on the new head
  check_collision();
  check_apple();
}

void check_collision(void) {
  // Check if the head of the snake is equal to one of its body pixels
  // If so, end the game with a loss
}

void check_apple(void) {
  // Check if head of the snake is equal to apple pixel
  // If so, extend snake by one pixel
  // Then, if size of snake is max length, end the game with a win
  // If not, spawn a new apple
  }