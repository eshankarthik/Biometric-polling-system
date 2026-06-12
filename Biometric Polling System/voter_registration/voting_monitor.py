import serial
import csv
import cv2
import os

SERIAL_PORT = 'COM3'
BAUD_RATE = 115200

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

print("Waiting for fingerprint match...")

current_window_open = False

while True:

    if ser.in_waiting:

        data = ser.readline().decode().strip()

        # =====================================
        # ENCRYPTION CODE
        # =====================================

        if data.startswith("ENC:"):

            code = data.replace("ENC:", "")

            print("Encryption Code:", code)

            continue

        # =====================================
        # VOTE COMPLETED
        # =====================================

        if data == "VOTE_DONE":

            print("Vote Completed")

            if current_window_open:

                cv2.destroyAllWindows()

                current_window_open = False

            continue

        # =====================================
        # IGNORE NON-NUMBERS
        # =====================================

        if not data.isdigit():
            continue

        fingerprint_id = data

        print("\nFingerprint Verified")
        print("Fingerprint ID:", fingerprint_id)

        with open('voters.csv', 'r') as file:

            reader = csv.DictReader(file)

            for row in reader:

                if row['FingerprintID'] == fingerprint_id:

                    print("Name :", row['Name'])
                    print("UID  :", row['UID'])

                    image_path = row['PhotoPath']

                    if os.path.exists(image_path):

                        image = cv2.imread(image_path)

                        cv2.imshow(
                            row['Name'],
                            image
                        )

                        current_window_open = True

                        cv2.waitKey(1)

                    break

    cv2.waitKey(1)