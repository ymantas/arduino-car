/*
 * Project: Smart Obstacle Avoiding Robot (Autonomous Navigation)
 * Platform: Arduino Uno R3
 * Driver: L293D (Current Setup) -> Pending Upgrade to L298N
 * * Περιγραφή Λειτουργίας:
 * Το σύστημα ελέγχει συνεχώς την απόσταση από εμπόδια. Σε περίπτωση εντοπισμού (<30cm),
 * το όχημα ακινητοποιείται, σαρώνει τον χώρο δεξιά και αριστερά μέσω του Servo
 * και αποφασίζει την βέλτιστη πορεία διαφυγής.
 */

#include <Servo.h> 

// --- 1. Ορισμός Συνδέσεων (Pin Configuration) ---

// Αισθητήρας Υπερήχων (Sonar)
const int trigPin = 12; 
const int echoPin = 13; 

// Micro Servo (Μηχανισμός Σάρωσης)
const int servoPin = 11;

// Κινητήρες (L293D Wiring Config)
// Αριστερή Πλευρά
const int motorLeft_Pin1 = 2;  // L293D Pin 2
const int motorLeft_Pin2 = 3;  // L293D Pin 7
// Δεξιά Πλευρά
const int motorRight_Pin1 = 4; // L293D Pin 10
const int motorRight_Pin2 = 7; // L293D Pin 15

// --- 2. Μεταβλητές Συστήματος ---
Servo headServo;   
long duration;     
int distance;      

void setup() {
  // Αρχικοποίηση Serial Monitor 
  Serial.begin(9600);
  Serial.println("System Initializing...");

  // Ρύθμιση Servo
  headServo.attach(servoPin);
  headServo.write(90); // Αρχική θέση: Ευθεία
  delay(500);

  // Ρύθμιση Pins Αισθητήρα
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Ρύθμιση Pins Κινητήρων
  pinMode(motorLeft_Pin1, OUTPUT);
  pinMode(motorLeft_Pin2, OUTPUT);
  pinMode(motorRight_Pin1, OUTPUT);
  pinMode(motorRight_Pin2, OUTPUT);

  Serial.println("System Ready. Autonomous Mode ON.");
}

void loop() {
  // 1. Ανάγνωση Περιβάλλοντος
  distance = readDistance();
  
  // 2. Έλεγχος Ασφαλείας
  if (distance > 0 && distance <= 30) {
    // --- ΣΕΝΑΡΙΟ ΕΜΠΟΔΙΟΥ (OBSTACLE DETECTED) ---
    Serial.println("Alert: Obstacle Detected!");
    
    stopCar();       // Άμεση Πέδηση
    delay(300);
    
    moveBackward();  // Ελιγμός απομάκρυνσης
    delay(400);
    stopCar();
    
    // Διαδικασία Σάρωσης (Scanning Sequence)
    int distRight = lookRight();
    delay(500);
    int distLeft = lookLeft();
    delay(500);

    // Αλγόριθμος Απόφασης (Decision Making)
    if (distRight >= distLeft) {
      Serial.println("Decision: Turn RIGHT");
      turnRight();
      delay(700); 
    } else {
      Serial.println("Decision: Turn LEFT");
      turnLeft();
      delay(700); 
    }
    
    stopCar();
    
  } else {
    // --- ΚΑΝΟΝΙΚΗ ΠΟΡΕΙΑ (FORWARD) ---
    moveForward();
  }
  
  delay(50); // Σταθεροποίηση κύκλου
}

// ---------------------------------------------------------
//              ΒΟΗΘΗΤΙΚΕΣ ΣΥΝΑΡΤΗΣΕΙΣ (FUNCTIONS)
// ---------------------------------------------------------

// Μέτρηση Απόστασης (cm)
int readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

// Σάρωση Δεξιά
int lookRight() {
  headServo.write(10); 
  delay(700);          
  int dist = readDistance();
  headServo.write(90); 
  return dist;
}

// Σάρωση Αριστερά
int lookLeft() {
  headServo.write(170); 
  delay(700);
  int dist = readDistance();
  headServo.write(90);
  return dist;
}

// --- Logic Οδήγησης Κινητήρων ---

void moveForward() {
  digitalWrite(motorLeft_Pin1, HIGH);
  digitalWrite(motorLeft_Pin2, LOW);
  
  digitalWrite(motorRight_Pin1, HIGH);
  digitalWrite(motorRight_Pin2, LOW);
}

void moveBackward() {
  digitalWrite(motorLeft_Pin1, LOW);
  digitalWrite(motorLeft_Pin2, HIGH);
  
  digitalWrite(motorRight_Pin1, LOW);
  digitalWrite(motorRight_Pin2, HIGH);
}

void turnLeft() {
  // Αριστερό Πίσω - Δεξί Μπροστά
  digitalWrite(motorLeft_Pin1, LOW);
  digitalWrite(motorLeft_Pin2, HIGH);
  
  digitalWrite(motorRight_Pin1, HIGH);
  digitalWrite(motorRight_Pin2, LOW);
}

void turnRight() {
  // Αριστερό Μπροστά - Δεξί Πίσω
  digitalWrite(motorLeft_Pin1, HIGH);
  digitalWrite(motorLeft_Pin2, LOW);
  
  digitalWrite(motorRight_Pin1, LOW);
  digitalWrite(motorRight_Pin2, HIGH);
}

void stopCar() {
  digitalWrite(motorLeft_Pin1, LOW);
  digitalWrite(motorLeft_Pin2, LOW);
  digitalWrite(motorRight_Pin1, LOW);
  digitalWrite(motorRight_Pin2, LOW);
}