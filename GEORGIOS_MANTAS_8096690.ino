/*
 * Project: Autonomous Obstacle Avoiding Robot
 * Course: Embedded Systems Programming / Sensors
 * Hardware: Arduino UNO, L293D Driver, HC-SR04 Sensor, SG90 Servo, 4 DC Motors
 * * Description:
 * This code controls a 4WD robot car. It uses an Ultrasonic sensor to detect obstacles.
 * When an obstacle is detected (< 30cm), the robot stops, moves backward, scans the
 * environment (Right/Left) using a Servo motor, and decides the best path based on
 * available space. It uses a random turn duration to avoid getting stuck in loops.
 * * Student ID: 8096690
 */

#include <Servo.h> 

// --- 1. PIN CONFIGURATION ---

// Ultrasonic Sensor (HC-SR04)
// Connected to pins 12 and 13 to avoid conflicts with Servo timer
const int trigPin = 12; 
const int echoPin = 13; 

// Servo Motor (SG90)
const int servoPin = 11;

// Motor Driver L293D Connections
// Left Side Motors (Connected in parallel)
const int motorLeft_Pin1 = 2;  // Input 1
const int motorLeft_Pin2 = 3;  // Input 2
// Right Side Motors (Connected in parallel)
const int motorRight_Pin1 = 4; // Input 3
const int motorRight_Pin2 = 7; // Input 4

// --- 2. GLOBAL VARIABLES ---
Servo headServo;   // Servo object to control sensor direction
long duration;     // Variable to store time of flight of sound wave
int distance;      // Variable to store calculated distance

void setup() {
  // Initialize Serial Communication for debugging purposes
  Serial.begin(9600);
  Serial.println("System Initializing...");

  // Initialize Servo Motor
  headServo.attach(servoPin);
  headServo.write(90); // Set initial position to look forward (90 degrees)
  delay(500);

  // Initialize Sensor Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize Motor Pins as Outputs
  pinMode(motorLeft_Pin1, OUTPUT);
  pinMode(motorLeft_Pin2, OUTPUT);
  pinMode(motorRight_Pin1, OUTPUT);
  pinMode(motorRight_Pin2, OUTPUT);

  // Initialize Random Number Generator
  // Reads noise from an unconnected analog pin (A0) to generate a random seed
  randomSeed(analogRead(0));

  Serial.println("System Ready. Autonomous Mode with Random Turning Active.");
}

void loop() {
  // 1. SENSE: Read distance from the sensor
  distance = readDistance();
  
  // 2. THINK: Check if obstacle is too close (30 cm limit)
  if (distance <= 30) {
    // --- OBSTACLE DETECTED ---
    Serial.println("Alert: Obstacle Detected!");
    
    // Step A: Stop the car immediately
    stopCar();       
    delay(300);
    
    // Step B: Move backward to avoid collision and create space
    moveBackward();  
    delay(400);
    stopCar();
    
    // Step C: Scan the environment (Look Right and Left)
    int distRight = lookRight();
    delay(500);
    int distLeft = lookLeft();
    delay(500);

    // Print values for debugging
    Serial.print("Right Dist: "); Serial.print(distRight);
    Serial.print(" | Left Dist: "); Serial.println(distLeft);

    // Step D: Calculate Random Turn Time
    // Generates a random time between 400ms and 1100ms
    // This varies the turn angle approx. between 45 and 135 degrees
    int turnTime = random(400, 1100); 
    Serial.print("Turning time (ms): "); Serial.println(turnTime);

    // Step E: DECIDE and ACT
    // Compare distances and turn towards the clearer path
    if (distRight >= distLeft) {
      Serial.println("Decision: Turn RIGHT");
      turnRight();
      delay(turnTime); // Turn for the random duration
    } else {
      Serial.println("Decision: Turn LEFT");
      turnLeft();
      delay(turnTime); // Turn for the random duration
    }
    
    // Stop briefly before moving forward again
    stopCar();
    
  } else {
    // 3. NO OBSTACLE: Keep moving forward
    moveForward();
  }
  
  delay(50); // Small delay for system stability
}

// ---------------------------------------------------------
//              HELPER FUNCTIONS
// ---------------------------------------------------------

// Function to measure distance using the HC-SR04 sensor
int readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Send a 10 microsecond pulse to trigger the sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Measure the duration of the echo pulse
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in cm (Speed of sound is approx 340 m/s)
  int cm = duration * 0.034 / 2;

  // FIX FOR ZERO READINGS:
  // If the sensor returns 0, it means the object is out of range or signal was lost.
  // We treat this as "clear path" (250cm) to prevent erroneous stopping.
  if (cm == 0) {
    return 250;
  }
  return cm;
}

// Function to scan right
int lookRight() {
  headServo.write(10); // Rotate servo to 10 degrees
  delay(700);          // Wait for servo to reach position
  int dist = readDistance();
  headServo.write(90); // Return to center
  return dist;
}

// Function to scan left
int lookLeft() {
  headServo.write(170); // Rotate servo to 170 degrees
  delay(700);
  int dist = readDistance();
  headServo.write(90);  // Return to center
  return dist;
}

// --- Motor Control Functions ---

void moveForward() {
  // Left Motors Forward
  digitalWrite(motorLeft_Pin1, HIGH);
  digitalWrite(motorLeft_Pin2, LOW);
  // Right Motors Forward
  digitalWrite(motorRight_Pin1, HIGH);
  digitalWrite(motorRight_Pin2, LOW);
}

void moveBackward() {
  // Left Motors Backward
  digitalWrite(motorLeft_Pin1, LOW);
  digitalWrite(motorLeft_Pin2, HIGH);
  // Right Motors Backward
  digitalWrite(motorRight_Pin1, LOW);
  digitalWrite(motorRight_Pin2, HIGH);
}

void turnLeft() {
  // Left Motors Backward, Right Motors Forward
  digitalWrite(motorLeft_Pin1, LOW);
  digitalWrite(motorLeft_Pin2, HIGH);
  digitalWrite(motorRight_Pin1, HIGH);
  digitalWrite(motorRight_Pin2, LOW);
}

void turnRight() {
  // Left Motors Forward, Right Motors Backward
  digitalWrite(motorLeft_Pin1, HIGH);
  digitalWrite(motorLeft_Pin2, LOW);
  digitalWrite(motorRight_Pin1, LOW);
  digitalWrite(motorRight_Pin2, HIGH);
}

void stopCar() {
  // Stop all motors
  digitalWrite(motorLeft_Pin1, LOW);
  digitalWrite(motorLeft_Pin2, LOW);
  digitalWrite(motorRight_Pin1, LOW);
  digitalWrite(motorRight_Pin2, LOW);
}
