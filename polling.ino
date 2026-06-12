/*
====================================================
BIOMETRIC POLLING SYSTEM
ESP32 + R307 + LCD + NSK193 + Buzzer
====================================================

FEATURES
----------------------------------------------------
✓ Fingerprint Authentication
✓ One Vote Per Person
✓ Live Image Verification Support
✓ Hidden Vote Counting
✓ OTP-Protected Results
✓ OTP Changes Every 15 Seconds
✓ Image Stays Until Vote Completion
✓ Party-Based Voting
✓ Offline Secure Voting

====================================================
CONNECTIONS
====================================================

R307 Fingerprint Sensor
--------------------------------
TX  -> GPIO16
RX  -> GPIO17
VCC -> VIN
GND -> GND

I2C LCD
--------------------------------
SDA -> GPIO21
SCL -> GPIO22
VCC -> VIN
GND -> GND

NSK193 Buttons
--------------------------------
SW0 -> GPIO32
SW1 -> GPIO27
SW2 -> GPIO25
SW3 -> GPIO26
COM -> GND

Buzzer
--------------------------------
+ -> GPIO14
- -> GND

====================================================
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

// =====================================
// LCD
// =====================================

LiquidCrystal_I2C lcd(0x27, 16, 2);

// =====================================
// FINGERPRINT
// =====================================

HardwareSerial mySerial(2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// =====================================
// BUTTONS
// =====================================

#define BTN1 32
#define BTN2 27
#define BTN3 25
#define BTN4 26

// =====================================
// BUZZER
// =====================================

#define BUZZER 14

// =====================================
// DOUBLE VOTING PREVENTION
// =====================================

bool votedUsers[128] = {false};

int currentFingerprintID = -1;

// =====================================
// VOTE COUNTS
// =====================================

int party1Votes = 0;
int party2Votes = 0;
int party3Votes = 0;
int notaVotes = 0;

// =====================================
// OTP SECURITY
// =====================================

String currentOTP = "";

unsigned long lastOTPUpdate = 0;

// =====================================

void setup()
{
  Serial.begin(115200);

  // =====================================
  // LCD INIT
  // =====================================

  lcd.init();

  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Polling System");

  lcd.setCursor(0,1);
  lcd.print("Initializing");

  // =====================================
  // BUTTONS
  // =====================================

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);

  // =====================================
  // BUZZER
  // =====================================

  pinMode(BUZZER, OUTPUT);

  // =====================================
  // FINGERPRINT SERIAL
  // =====================================

  mySerial.begin(57600, SERIAL_8N1, 16, 17);

  finger.begin(57600);

  // =====================================
  // VERIFY SENSOR
  // =====================================

  if (finger.verifyPassword())
  {
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Sensor Found");

    beepShort();
  }
  else
  {
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Sensor Error");

    while (1);
  }

  // =====================================
  // INITIAL OTP
  // =====================================

  randomSeed(analogRead(34));

  generateOTP();

  delay(2000);

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Place Finger");
}

// =====================================

void loop()
{
  // =====================================
  // AUTO CHANGE OTP EVERY 15 SECONDS
  // =====================================

  if (millis() - lastOTPUpdate > 15000)
  {
    generateOTP();

    lastOTPUpdate = millis();
  }

  // =====================================
  // RESULT ACCESS
  // =====================================

  if (Serial.available())
  {
    String enteredCode = Serial.readStringUntil('\n');

    enteredCode.trim();

    if (enteredCode == currentOTP)
    {
      Serial.println();
      Serial.println("===== SECURE RESULTS =====");

      Serial.print("Party 1: ");
      Serial.println(party1Votes);

      Serial.print("Party 2: ");
      Serial.println(party2Votes);

      Serial.print("Party 3: ");
      Serial.println(party3Votes);

      Serial.print("NOTA: ");
      Serial.println(notaVotes);

      Serial.println("==========================");
      Serial.println();
    }
  }

  // =====================================
  // FINGERPRINT MATCH
  // =====================================

  int fingerprintID = getFingerprintID();

  currentFingerprintID = fingerprintID;

  if (fingerprintID != -1)
  {
    // =====================================
    // SEND ID TO PYTHON
    // =====================================

    Serial.println(fingerprintID);

    // =====================================
    // ALREADY VOTED CHECK
    // =====================================

    if (votedUsers[fingerprintID] == true)
    {
      lcd.clear();

      lcd.setCursor(0,0);
      lcd.print("ALREADY VOTED");

      lcd.setCursor(0,1);
      lcd.print("ACCESS DENIED");

      beepLong();

      delay(3000);

      lcd.clear();

      lcd.setCursor(0,0);
      lcd.print("Place Finger");

      return;
    }

    // =====================================
    // VERIFIED
    // =====================================

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Verified");

    lcd.setCursor(0,1);
    lcd.print("ID:");
    lcd.print(fingerprintID);

    beepShort();

    delay(2000);

    // =====================================
    // SHOW PARTIES
    // =====================================

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("1P1 2P2 3P3");

    lcd.setCursor(0,1);
    lcd.print("4NOTA Vote");

    bool voted = false;

    while (!voted)
    {
      // ================= PARTY 1 =================

      if (digitalRead(BTN1) == LOW)
      {
        party1Votes++;

        voteSuccess("Party 1");

        voted = true;
      }

      // ================= PARTY 2 =================

      if (digitalRead(BTN2) == LOW)
      {
        party2Votes++;

        voteSuccess("Party 2");

        voted = true;
      }

      // ================= PARTY 3 =================

      if (digitalRead(BTN3) == LOW)
      {
        party3Votes++;

        voteSuccess("Party 3");

        voted = true;
      }

      // ================= NOTA =================

      if (digitalRead(BTN4) == LOW)
      {
        notaVotes++;

        voteSuccess("NOTA");

        voted = true;
      }
    }

    delay(3000);

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Place Finger");
  }
}

// =====================================
// OTP GENERATION
// =====================================

void generateOTP()
{
  int randomNumber = random(100000, 999999);

  currentOTP = String(randomNumber);

  Serial.print("ENC:");
  Serial.println(currentOTP);
}

// =====================================
// FINGERPRINT FUNCTION
// =====================================

int getFingerprintID()
{
  uint8_t p = finger.getImage();

  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.image2Tz();

  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.fingerSearch();

  if (p != FINGERPRINT_OK)
  {
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Access Denied");

    beepLong();

    delay(2000);

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Place Finger");

    return -1;
  }

  return finger.fingerID;
}

// =====================================
// VOTE SUCCESS
// =====================================

void voteSuccess(String party)
{
  // =====================================
  // MARK USER AS VOTED
  // =====================================

  votedUsers[currentFingerprintID] = true;

  // =====================================
  // INFORM PYTHON
  // =====================================

  Serial.println("VOTE_DONE");

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Vote Casted");

  lcd.setCursor(0,1);
  lcd.print(party);

  beepDouble();
}

// =====================================
// BUZZER FUNCTIONS
// =====================================

void beepShort()
{
  digitalWrite(BUZZER, HIGH);

  delay(100);

  digitalWrite(BUZZER, LOW);
}

void beepLong()
{
  digitalWrite(BUZZER, HIGH);

  delay(1000);

  digitalWrite(BUZZER, LOW);
}

void beepDouble()
{
  digitalWrite(BUZZER, HIGH);

  delay(100);

  digitalWrite(BUZZER, LOW);

  delay(100);

  digitalWrite(BUZZER, HIGH);

  delay(100);

  digitalWrite(BUZZER, LOW);
}