import serial
import cv2
import csv
import os
import time
import random

# =====================================
# SERIAL SETTINGS
# =====================================

SERIAL_PORT = 'COM3'   # CHANGE IF NEEDED
BAUD_RATE = 115200

# =====================================
# CREATE PHOTOS FOLDER
# =====================================

if not os.path.exists('photos'):
    os.makedirs('photos')

# =====================================
# CONNECT TO ESP32
# =====================================

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

time.sleep(2)

# =====================================
# GENERATE RANDOM UID
# =====================================

uid = "VTR" + str(random.randint(100000, 999999))

# =====================================
# USER INPUT
# =====================================

print("===== VOTER REGISTRATION =====")

name = input("Enter Voter Name: ")

print("Generated UID:", uid)

fingerprint_id = input("Enter Fingerprint ID (1-127): ")

# =====================================
# SEND ID TO ESP32
# =====================================

ser.write((fingerprint_id + '\n').encode())

print("\nWaiting for fingerprint enrollment...\n")

# =====================================
# WAIT FOR ENROLLMENT
# =====================================

while True:

    if ser.in_waiting:

        msg = ser.readline().decode().strip()

        print(msg)

        # =====================================
        # RETRY IF FAILED
        # =====================================

        if msg == "FAILED":

            print("\nFingerprint failed.")
            print("Please place finger properly again.\n")

        # =====================================
        # SUCCESS
        # =====================================

        if msg == "SUCCESS":

            print("\nFingerprint Enrolled Successfully!")

            break

# =====================================
# OPEN WEBCAM
# =====================================

cap = cv2.VideoCapture(0)

print("\nPress SPACE to capture image")

while True:

    ret, frame = cap.read()

    cv2.imshow("Capture Voter Image", frame)

    key = cv2.waitKey(1)

    # SPACE KEY
    if key == 32:

        image_path = f"photos/{uid}.jpg"

        cv2.imwrite(image_path, frame)

        print("Image Saved")

        break

# =====================================
# CLOSE CAMERA
# =====================================

cap.release()

cv2.destroyAllWindows()

# =====================================
# SAVE VOTER DATA
# =====================================

file_exists = os.path.isfile('voters.csv')

with open('voters.csv', 'a', newline='') as file:

    writer = csv.writer(file)

    if not file_exists:

        writer.writerow([
            'Name',
            'UID',
            'FingerprintID',
            'PhotoPath'
        ])

    writer.writerow([
        name,
        uid,
        fingerprint_id,
        image_path
    ])

# =====================================
# FINISHED
# =====================================

print("\n================================")
print("Voter Registered Successfully!")
print("================================")

print("Name :", name)
print("UID  :", uid)
print("Fingerprint ID :", fingerprint_id)