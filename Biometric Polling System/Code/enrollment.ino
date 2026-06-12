/*
====================================================
R307 FINGERPRINT ENROLLMENT SYSTEM
ESP32 + R307
====================================================

PURPOSE
----------------------------------------------------
Used ONLY for:
✓ Enrolling new fingerprints
✓ Working with register.py
✓ Capturing voter image through Python

DO NOT use this during voting.

====================================================
CONNECTIONS
====================================================

R307 Fingerprint Sensor
--------------------------------
TX  -> GPIO16
RX  -> GPIO17
VCC -> VIN
GND -> GND

====================================================
*/

#include <Adafruit_Fingerprint.h>

// =====================================
// FINGERPRINT SERIAL
// =====================================

HardwareSerial mySerial(2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// =====================================

uint8_t id;

// =====================================

void setup()
{
  Serial.begin(115200);

  // =====================================
  // START FINGERPRINT SERIAL
  // =====================================

  mySerial.begin(57600, SERIAL_8N1, 16, 17);

  finger.begin(57600);

  // =====================================
  // VERIFY SENSOR
  // =====================================

  if (finger.verifyPassword())
  {
    Serial.println("READY");
  }
  else
  {
    Serial.println("FINGERPRINT SENSOR ERROR");

    while (1);
  }
}

// =====================================

void loop()
{
  // =====================================
  // WAIT FOR FINGERPRINT ID FROM PYTHON
  // =====================================

  if (Serial.available())
  {
    id = Serial.parseInt();

    if (id != 0)
    {
      enrollFingerprint(id);
    }
  }
}

// =====================================
// ENROLL FINGERPRINT
// =====================================

uint8_t enrollFingerprint(uint8_t id)
{
  int p = -1;

  // =====================================
  // FIRST SCAN
  // =====================================

  Serial.println("PLACE_FINGER");

  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();

    if (p == FINGERPRINT_NOFINGER)
    {
      continue;
    }

    if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
      Serial.println("FAILED");

      return p;
    }

    if (p == FINGERPRINT_IMAGEFAIL)
    {
      Serial.println("FAILED");

      return p;
    }
  }

  p = finger.image2Tz(1);

  if (p != FINGERPRINT_OK)
  {
    Serial.println("FAILED");

    return p;
  }

  // =====================================
  // REMOVE FINGER
  // =====================================

  Serial.println("REMOVE_FINGER");

  delay(2000);

  p = 0;

  while (p != FINGERPRINT_NOFINGER)
  {
    p = finger.getImage();
  }

  // =====================================
  // SECOND SCAN
  // =====================================

  Serial.println("PLACE_AGAIN");

  p = -1;

  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();

    if (p == FINGERPRINT_NOFINGER)
    {
      continue;
    }

    if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
      Serial.println("FAILED");

      return p;
    }

    if (p == FINGERPRINT_IMAGEFAIL)
    {
      Serial.println("FAILED");

      return p;
    }
  }

  p = finger.image2Tz(2);

  if (p != FINGERPRINT_OK)
  {
    Serial.println("FAILED");

    return p;
  }

  // =====================================
  // CREATE MODEL
  // =====================================

  p = finger.createModel();

  if (p != FINGERPRINT_OK)
  {
    Serial.println("FAILED");

    return p;
  }

  // =====================================
  // STORE MODEL
  // =====================================

  p = finger.storeModel(id);

  if (p == FINGERPRINT_OK)
  {
    Serial.println("SUCCESS");
  }
  else
  {
    Serial.println("FAILED");
  }

  return p;
}