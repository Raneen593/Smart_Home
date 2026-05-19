#include <Keypad.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// ===== Software Serial ======
// SoftwareSerial bt(0, 1);
// SoftwareSerial ard2(A2, A3);  // rx, tx

// ===== LCD =====
// LiquidCrystal_I2C lcd(0x3F, 16, 2);   // لو مشتغلتش جرّبي 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== Servo =====
Servo door;
// Servo umbrella;

// ===== Keypad size =====
const byte rows = 4;
const byte cols = 4;

// ===== Keypad layout =====
char keys[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// ===== Keypad pins (ROWS first 13 → 10, COLS 9 → 6) =====
byte rowPins[rows] = {  9, 8, 7, 6 };
byte colPins[cols] = {5,4,3,2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

// ===== Password =====
String password = "4444";
String input = "";

int maxTries = 2;
int tries = 0;

// ===== Buzzer =====
#define buzzer 12
int Sirenstate = 0;

// // ===== Rain ======
// #define rain 13

#define btn A0
void setup() {
  // Servo
  door.attach(10);
  // umbrella.attach(11);
  door.write(90);  // الباب مقفول
  // umbrella.write(0);
  // Buzzer
  pinMode(buzzer, OUTPUT);
    // Touch Sensor
  pinMode(btn, INPUT);
  // pinMode(rain, INPUT);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password");
  Serial.begin(9600);
  bt.begin(9600);
  // ard2.begin(9600);
}

void loop() {
  bluetooth();
  checkPassword();
  touchSensor();
  

}

int colCounter = 0;

void checkPassword() {
  char key = keypad.getKey();
  lcd.setCursor(0, 1);

  if (key) {
    // Serial.println(key);
    input += key;
    lcd.setCursor(colCounter, 1);
    lcd.print("*");
    colCounter++;
    tone(buzzer, 1000);  // صوت 1kHz
    delay(100);
    noTone(buzzer);

    if (input.length() == 4) {
      Serial.println(input);
      
      if (input == password) {
        lcd.clear();
        lcd.print("Correct!");
        successSound();
        delay(1000);
        lcd.clear();
        openServo(door);
        lcd.print("Door Opened!");
        delay(5000);
        input = "";
        colCounter = 0;
        tries = 0;
        closeServo(door);
        showEnterPassword();
        checkPassword();
      } else {
        if (tries != maxTries) {
          lcd.clear();
          lcd.print("Wrong!!");
          lcd.setCursor(0, 1);
          lcd.print(maxTries - tries);
          lcd.print(" Tries left");
          buzzerWrong();
          input = "";
          colCounter = 0;
          tries++;
          showEnterPassword();
          checkPassword();
        } else {
          lcd.clear();
          lcd.print("haraaaaaaamy!!");
          startSiren();
          tries = 0;
          input = "";
          colCounter = 0;
          showEnterPassword();
          checkPassword();
        }
      }
    }
  }
}

void showEnterPassword() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password: ");
}

void openServo(Servo servo) {
  servo.write(0);
}

void closeServo(Servo servo) {
  servo.write(90);
}

void buzzerWrong() {
  for (int i = 0; i < 3; i++) {
   
  // ha
  tone(buzzer, 700);
  delay(200);
  noTone(buzzer);
  delay(80);

  // raa
  tone(buzzer, 900);
  delay(350);
  noTone(buzzer);
  delay(80);

  // my
  tone(buzzer, 1100);
  delay(250);
  noTone(buzzer);
  delay(300);

  // تكرار الكلمة (حرامي تاني)
  tone(buzzer, 700);
  delay(200);
  noTone(buzzer);
  delay(80);

  tone(buzzer, 900);
  delay(350);
  noTone(buzzer);
  delay(80);

  tone(buzzer, 1100);
  delay(250);
  noTone(buzzer);
  }
}

void successSound() {
  tone(buzzer, 800);
  delay(150);
  tone(buzzer, 1200);
  delay(150);
  tone(buzzer, 1600);
  delay(200);
  noTone(buzzer);
}

void startSiren() {
  unsigned long startTime = millis();  // وقت بداية السيرنة

  while (millis() - startTime < 5000) {
    // تصاعد
    for (int i = 1000; i < 2000; i += 50) {
      tone(buzzer, i);
      delay(20);
      // تحقق لو انتهت المدة
      if (millis() - startTime >= 5000) break;
    }
    // تنازل
    for (int i = 2000; i > 1000; i -= 50) {
      tone(buzzer, i);
[5/11/2026 8:31 PM] Dalia Alagawy: delay(20);
      if (millis() - startTime >= 5000) break;
    }
  }

  noTone(buzzer);  // وقف الصوت بعد المدة
}

int umbrellaOpen = true;

// void rainSensor() {
//   int rainRead = digitalRead(rain);
//   if (!rainRead && !umbrellaOpen) {  // لو في مطر ولسه مش مفتوحة
//     for (int i = 0; i <= 100; i++) {
//       umbrella.write(i);
//       delay(15);
//     }
//     umbrellaOpen = true;
//   }
//   else if (rainRead && umbrellaOpen) {  // لو مفيش مطر ولسه مفتوحة
//     for (int i = 90; i >= 0; i--) {
//       umbrella.write(i);
//       delay(15);
//     }
//     umbrellaOpen = false;
//   }
// }
// ===== Touch Sensor Function =====
void touchSensor() {

int state = digitalRead(btn);
  // جربي HIGH ولو مشتغلش غيريها LOW
  if (state == HIGH) {
      bellSound();
  }
}

// ===== Bell Sound =====
void bellSound() {

  tone(buzzer,1200);
  delay(200);

  tone(buzzer,1500);
  delay(200);

  tone(buzzer,1800);
  delay(300);

  noTone(buzzer);
}


void bluetooth() {
  if (bt.available() > 0) {
    String data = bt.readString();
    data.toLowerCase();


    // open gate
    if (data == "open the door") {
      openServo(door);
    } else if (data == "close the door") {
      closeServo(door);
    }  else if (data == "turn on light") {
      Serial.write("light on");
    } else if (data == "turn off light") {
      Serial.write("light off");
    } 
  }
}
