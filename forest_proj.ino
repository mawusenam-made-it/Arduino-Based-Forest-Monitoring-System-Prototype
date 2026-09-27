#include <SoftwareSerial.h>

// GSM SIM800L Serial
SoftwareSerial gsm(6, 5); // RX, TX

// Pin assignments
const int soundSensorPin = A0;
const int flamePin = 8;
const int pirPin = 4;
const int trigPin = 12;
const int echoPin = 11;
const int buzzerPin = 2;

const int threshold = 600; // Sound threshold
String phoneNumber = "+233592795343"; // 

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);

  pinMode(soundSensorPin, INPUT);
  pinMode(flamePin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(buzzerPin, LOW);

  Serial.println("System Initialized...");
  delay(2000); // Give time for GSM to stabilize
}

void loop() {
  bool alertTriggered = false;
  String alertMessage = "";

  // 1. Sound Sensor
  int soundLevel = analogRead(soundSensorPin);
  if (soundLevel > threshold) {
    Serial.println("🔊 Loud sound detected!");
    alertMessage += "🔊 Loud sound detected.\n";
    alertTriggered = true;
    delay(500);
  }

  // 2. Flame Sensor
  if (digitalRead(flamePin) == LOW) {
    Serial.println("🔥 Flame detected!");
    digitalWrite(buzzerPin, HIGH);
    alertMessage += "🔥 Flame detected!\n";
    alertTriggered = true;
  } else {
    digitalWrite(buzzerPin, LOW);
  }

  // 3. PIR Sensor
  if (digitalRead(pirPin) == HIGH) {
    Serial.println("👣 Motion detected!");
    alertMessage += "👣 Motion detected!\n";
    alertTriggered = true;
  }

  // 4. Ultrasonic Distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = (duration * 0.0343) / 2;

  if (distance < 50.0 && distance > 0) {
    Serial.print("📏 Object too close! Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    alertMessage += "📏 Object too close: " + String(distance) + " cm\n";
    alertTriggered = true;
  }

  // If any alert was triggered, send SMS
  if (alertTriggered) {
    Serial.println("🚨 ALERT!");
    Serial.println("--------------------------");
    sendSMS(alertMessage);
  }

  delay(200);
}

void sendSMS(String message) {
  gsm.println("AT+CMGF=1");               // Set SMS mode
  delay(500);
  gsm.println("AT+CMGS=\"" + phoneNumber + "\"");
  delay(500);
  gsm.print(message);                    
  delay(500);
  gsm.write(26);                         
  delay(2000);

  Serial.println("ALERT SENT.");
}
