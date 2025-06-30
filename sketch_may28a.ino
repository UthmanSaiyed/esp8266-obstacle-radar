#define BLYNK_TEMPLATE_ID "TMPL5XPQbOBmi"
#define BLYNK_TEMPLATE_NAME "Radar Sensor"
#define BLYNK_AUTH_TOKEN "rqItAUuGpFIwyrKLtE6grmT6WLqbsm5U"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

// Wi-Fi credentials 
char ssid[] = "GalaxyA52s5G9A16";  
char pass[] = "12345678";

// Pins
#define TRIG_PIN 5   // D1
#define ECHO_PIN 4   // D2
#define SERVO_PIN 14 // D5

Servo myServo;

int currentAngle = 0;
int direction = 1;
unsigned long lastSweepTime = 0;
const unsigned long sweepDelay = 150;

float lastDistance = 0;

BlynkTimer timer;

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println();
  Serial.println("Booting...");

  myServo.attach(SERVO_PIN);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(500L, sendToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();
  updateRadarSweep();
}

void updateRadarSweep() {
  unsigned long currentTime = millis();
  if (currentTime - lastSweepTime >= sweepDelay) {
    myServo.write(currentAngle);
    float distance = measureDistance();

    lastDistance = distance;

    Serial.print("Angle: ");
    Serial.print(currentAngle);
    Serial.print("°, Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    currentAngle += 15 * direction;
    if (currentAngle >= 180 || currentAngle <= 0) {
      direction *= -1; // reverse direction
    }

    lastSweepTime = currentTime;
  }
}

void sendToBlynk() {
  Blynk.virtualWrite(V0, currentAngle);
  Blynk.virtualWrite(V1, lastDistance);
}

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000); // timeout after 25ms
  if (duration == 0) return -1;
  return (duration * 0.0343) / 2; // in cm
}