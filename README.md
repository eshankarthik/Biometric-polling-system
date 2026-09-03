# Biometric Polling System

A secure biometric voting system designed to authenticate voters quickly and reliably, using fingerprint verification combined with identity validation.

## Overview

Our team built this system to explore how biometric authentication could make voting both faster and more secure. Using an R307 fingerprint sensor paired with an ESP32, the system authenticates voters in under 1 second, while a webcam-based identity check adds a second layer of verification.

## Features

- Fingerprint-based voter authentication (under 1 second)
- Webcam-based identity validation as a secondary security layer
- Python backend with OTP-based dynamic refresh codes to prevent code reuse
- Offline vote counting with local database sync

## My Role

I was responsible for the hardware side of the project — connecting and calibrating the R307 fingerprint sensor with the ESP32 — and helped implement parts of the Python backend logic.

## Challenges & Approach

One challenge was ensuring voting codes couldn't be reused or intercepted; through discussion as a team, we arrived at an OTP-based dynamic refresh approach on the backend. Another challenge was keeping vote counting reliable without a constant internet connection — we addressed this by optimizing the local database logic to sync and reconcile votes once connectivity returned, refining it through several rounds of testing together.

## Hardware & Tools Used

- R307 Fingerprint Sensor
- ESP32
- Webcam (for identity validation)
- Python (backend)

## What I Learned

This project taught me how much planning goes into securing a system end-to-end, and how much stronger a solution becomes when the team works through problems together rather than individually.
