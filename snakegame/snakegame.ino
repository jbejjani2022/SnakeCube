#include <Adafruit_BNO08x.h>
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

// Max snake length
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
  // int length; // length of snake
  // int body[MAX_LENGTH][2]; // array of [face_id, pixel_id] pairs
  LinkedList<pixel> body = LinkedList<pixel>();
  char direction; // 'u' = UP, 'd' = DOWN, 'l' = LEFT, 'r' = RIGHT
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
  Serial.println(snake.body.size());

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
  check_collision();
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
}

void check_collision(void) {
  // Check if the head of the snake is equal to one of its body pixels
}