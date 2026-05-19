#include <Servo.h>
#include <SoftwareSerial.h>

// ===== Bluetooth =====
SoftwareSerial BT(A4, A5); // RX, TX

// ===== Sensors =====
#define flamePin 3
#define gaspin A0
#define rain 13
int threshold = 300;

// ===== Buzzer =====
#define buzzer 9

// ===== IR Sensor + LED =====
#define IR1 4
#define led1 8

// ===== Parking Sensors =====
#define IR_P1 5
#define IR_P2 7

// ===== Servo Motors =====
Servo umbrella;
Servo gateServo;

// ===== Bluetooth light control =====
bool lightBT = false;

// ===== Alarm States =====
bool gasDetected = false;
bool flameDetected = false;

// ===== Previous States =====
bool lastGasState = false;
bool lastFlameState = false;

// ===== Parking =====
bool slot1Empty = false;
bool slot2Empty = false;

// ===== Umbrella =====
bool umbrellaOpen = false;

// ===== IR Timer =====
unsigned long offTimer = 0;
bool ledState = false;

// ===== Flame Mode =====
// غيري القيمة دي لو الحساس بتاعك عكسي
bool flameActiveLow = true;

void setup() {
  pinMode(flamePin, INPUT);
  pinMode(gaspin, INPUT);
  pinMode(rain, INPUT);

  pinMode(buzzer, OUTPUT);

  pinMode(IR1, INPUT);
  pinMode(led1, OUTPUT);

  pinMode(IR_P1, INPUT);
  pinMode(IR_P2, INPUT);

  umbrella.attach(11);
  umbrella.write(180);

  gateServo.attach(12);
  gateServo.write(90);

  Serial.begin(9600);   // Debug
  BT.begin(9600);
}

void loop() {
  gassensor();
  flameSensor();
  controlBuzzer();   // 👈 حل التعارض
  irSensors();
  rainSensor();
  parkingSystem();
  receiveMessage();
  sendAlerts();
}

// =====================
// 🧪 Gas Sensor
// =====================
void gassensor() {
  int gasValue = analogRead(gaspin);

  if (gasValue > threshold) {
    gasDetected = true;
  } else {
    gasDetected = false;
  }
}

// =====================
// 🔥 Flame Sensor
// =====================
void flameSensor() {
  int flameValue = digitalRead(flamePin);

  Serial.print("Flame: ");
  Serial.println(flameValue);

  if (flameActiveLow) {
    flameDetected = (flameValue == LOW);
  } else {
    flameDetected = (flameValue == HIGH);
  }
}

// =====================
// 🔊 Buzzer Control
// =====================
void controlBuzzer() {
  if (gasDetected || flameDetected) {
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }
}

// =====================
// 👁 IR Motion Sensor
// =====================
void irSensors() {
  int Sensor1 = digitalRead(IR1);

  if (!Sensor1 && !lightBT) {
    digitalWrite(led1, HIGH);
    ledState = true;
    offTimer = millis();
  }

  if (ledState && (millis() - offTimer >= 5000)) {
    digitalWrite(led1, LOW);
    ledState = false;
  }
}

// =====================
// 🌧 Rain Sensor
// =====================
void rainSensor() {
  int rainRead = digitalRead(rain);

  if (!rainRead && !umbrellaOpen) {
    umbrella.write(90);
    umbrellaOpen = true;
  } 
  else if (rainRead && umbrellaOpen) {
    umbrella.write(180);
    umbrellaOpen = false;
  }
}

// =====================
// 🅿 Parking System
// =====================
void parkingSystem() {
  slot1Empty = digitalRead(IR_P1); 
  slot2Empty = digitalRead(IR_P2);
}

// =====================
// 🚪 Gate Control
// =====================
void openGate() {
  gateServo.write(0);
  delay(300);
  gateServo.write(90);
}

void closeGate() {
  gateServo.write(180);
  delay(300);
  gateServo.write(90);
}

// =====================
// 📶 Bluetooth Commands
// =====================
void receiveMessage() {
  if (BT.available() > 0) {
    String data = BT.readStringUntil('\n');
    data.trim();
    data.toLowerCase();

    if (data == "open") {
      openGate();
    }
    else if (data == "close") {
      closeGate();
      BT.println("Gate Closed");
    }
    else if (data == "status") {

      int emptySlots = 0;
      if (slot1Empty) emptySlots++;
      if (slot2Empty) emptySlots++;

      BT.println("===== Garage Status =====");
      BT.print("Empty Slots: ");
      BT.println(emptySlots);

      if (emptySlots > 0) {
        BT.println("Parking Available");
      } else {
        BT.println("Parking Full");
      }

      if (gasDetected) {
        BT.println("WARNING
[5/11/2026 8:32 PM] Dalia Alagawy: : Gas Leak!");
      } else {
        BT.println("Gas Normal");
      }

      if (flameDetected) {
        BT.println("WARNING: Fire!");
      } else {
        BT.println("No Fire");
      }
    }
    else if (data == "light on") {
      digitalWrite(led1, HIGH);
      lightBT = true;
      BT.println("Light ON");
    }
    else if (data == "light off") {
      digitalWrite(led1, LOW);
      lightBT = false;
      BT.println("Light OFF");
    }
  }
}

// =====================
// 🚨 Automatic Alerts
// =====================
void sendAlerts() {

  if (gasDetected && !lastGasState) {
    BT.println("⚠️ WARNING: Gas Leak Detected!");
  }

  if (!gasDetected && lastGasState) {
    BT.println("Gas Back To Normal");
  }

  if (flameDetected && !lastFlameState) {
    BT.println("🔥 WARNING: Fire Detected!");
  }

  if (!flameDetected && lastFlameState) {
    BT.println("Fire Cleared");
  }

  lastGasState = gasDetected;
  lastFlameState = flameDetected;
}
