#include <Servo.h>

const int Delay = 10;
const int potPin = A0;
int potState, potStatePrev;

// Servo setup
Servo servos[6];
int positions[6] = {0, 90, 90, 90, 90, 120};  // base, lower, mid, upper, grip

// Servo pin assignments
const int servoPins[6] = {8, 9, 11, 12, 13, 10}; // last one (10) used for lower2 mirror

// Button pins: {down1, up1, down2, up2, ...}
const int buttonPins[8] = {7, 6, 5, 4, 3, 2, A3, A4};

// Helper to read all buttons
int buttonStates[8];

void setup() {
  for (int i = 0; i < 8; i++) pinMode(buttonPins[i], INPUT);

  // Attach servos
  servos[0].attach(servoPins[0], 600, 2100);  // base
  servos[1].attach(servoPins[1], 600, 2100);  // lower1
  servos[2].attach(servoPins[2], 600, 2100);  // mid
  servos[3].attach(servoPins[3], 600, 2100);  // upper
  servos[4].attach(servoPins[4], 600, 2400);  // grip
  servos[5].attach(servoPins[5], 600, 2100);  // lower2 (mirror)

  updateServos();
  potState = analogRead(potPin);
  potStatePrev = potState;
}

void loop() {
  potState = analogRead(potPin);

  if (potState != potStatePrev) {
    positions[0] = map(potState, 0, 1023, 0, 179);  // base
    servos[0].write(positions[0]);
    delay(Delay);
    potStatePrev = potState;
  }

  // Read buttons
  for (int i = 0; i < 8; i++) {
    buttonStates[i] = digitalRead(buttonPins[i]);
  }

  // Dual servo (lower1 & lower2)
  adjustDualServo(1, 5, buttonStates[0], buttonStates[1]); // lower joint

  // Single servos: mid, upper, grip
  adjustServo(2, buttonStates[2], buttonStates[3]);  // mid
  adjustServo(3, buttonStates[4], buttonStates[5]);  // upper
  adjustServo(4, buttonStates[6], buttonStates[7], 60);  // grip, min=60
}

void updateServos() {
  for (int i = 0; i < 5; i++) {
    servos[i].write(positions[i]);
  }
  servos[5].write(180 - positions[1]);  // lower2 mirrors lower1
}

// Single servo control
void adjustServo(int index, int down, int up, int minVal = 0, int maxVal = 180) {
  if (up == HIGH) {
    positions[index]++;
    if (positions[index] > maxVal) positions[index] = maxVal;
    servos[index].write(positions[index]);
    delay(Delay);
  }
  if (down == HIGH) {
    positions[index]--;
    if (positions[index] < minVal) positions[index] = minVal;
    servos[index].write(positions[index]);
    delay(Delay);
  }
}

// Dual servo control (for lower1 + lower2 mirror)
void adjustDualServo(int index, int mirrorIndex, int down, int up, int minVal = 4, int maxVal = 175) {
  if (up == HIGH) {
    positions[index]++;
    if (positions[index] > maxVal) positions[index] = maxVal;
  }
  if (down == HIGH) {
    positions[index]--;
    if (positions[index] < minVal) positions[index] = minVal;
  }
  servos[index].write(positions[index]);
  servos[mirrorIndex].write(180 - positions[index]);
  delay(Delay);
}
