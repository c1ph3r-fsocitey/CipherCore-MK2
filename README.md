# ⬡ CipherCore MK2
### A Wearable Biometric Arc Reactor & Hackable CTF Badge

> *"Every engineer has a Tony Stark moment. This is mine."*

<img width="507" height="632" alt="IMG_20260603_132437" src="https://github.com/user-attachments/assets/d349b5c0-4f33-4a39-86eb-adf928bdf424" />

https://github.com/user-attachments/assets/0b99cb09-beab-414d-a055-732cde7b078b 

---

## The Reason Behind This Project

Every engineer is obsessed with Tony Stark — and me being a robotics engineer who's already built the **SO-ARM 101** (Tony's Dum-E robotic arm), a hardware hacker, and a cybersecurity conference speaker at events like **DEF CON, BSides, and Seasides**, I didn't want to stop there.

I wanted a project that lived at the intersection of all three worlds: **hardware, biometrics, and security.**

The result is **CipherCore MK2** — a 3D-printed EndGame-style arc reactor worn on my chest, housing an **EXG Pill** capturing my live ECG signal, a **XIAO ESP32S3** processing my heartbeat in real time, and a **WS2812 LED ring** that pulses in sync with my actual heartbeat.

But it doesn't stop there. CipherCore is also a **fully functional CTF (Capture The Flag) badge** — it broadcasts a WiFi access point with a 5-level web hacking challenge that conference attendees attempt to crack while I'm **wearing it**.

---

## 🌍 Is It Solving a Real-World Problem?

Yes — two problems simultaneously:

**1. Wearable Biometric Feedback**
Most ECG devices are clinical, bulky, or locked behind proprietary apps. CipherCore demonstrates that real-time ECG processing and intuitive biometric visualization (heartbeat-synced LEDs) can be built into creative everyday wearables at low cost using open hardware.

**2. Hands-On Security Education**
The cybersecurity community often lacks engaging, beginner-friendly CTF experiences. CipherCore turns a wearable device into a live security challenge — teaching real vulnerability classes in an exciting, memorable format that has never been done at a conference before.

The proof-of-pwn is **visible on the wearer's chest in real time.**

---

## 🛠️ Materials Required

### Electronics
| Component | Purpose |
|-----------|---------|
| EXG Pill (Upside Down Labs) | ECG analog front end |
| Seeed Studio XIAO ESP32S3 | Microcontroller + WiFi/BLE |
| WS2812B LED Strip (15 LEDs) | Heartbeat-synced light ring |
| ECG Snap Electrodes (3x) | Body contact for ECG signal |
| 20,000mAh USB Power Bank | Wearable power supply |
| Connecting wires + headers | Wiring |

### Housing & Wearable
| Component | Purpose |
|-----------|---------|
| 3D Printed Arc Reactor Housing | Modified EndGame-style chassis |
| Elastic Chest Harness + Plastic Base | Wearable mounting |
| Neodymium Magnets | Tool-free attachment/detachment |

### Software & Tools
- PlatformIO + Arduino framework
- Adafruit NeoPixel library
- ESP32 WebServer + DNSServer libraries

---

## 📋 Procedure

### Step 1 — ECG Signal Pipeline

The **EXG Pill** captures raw ECG from three snap electrodes on the chest. The analog output feeds into the XIAO ESP32S3's ADC at 12-bit resolution.

A **4-stage IIR biquad bandpass filter** (5–15Hz passband) isolates the QRS complex — the sharp spike in the ECG waveform that corresponds to each heartbeat. A 50Hz notch filter stage removes Indian mains interference.

```
Raw ECG → ADC (12-bit) → IIR Bandpass Filter → Rectify → Adaptive Threshold → R-Peak Detection
```

**Adaptive Thresholding** — the firmware tracks a rolling peak envelope and noise floor of the rectified filtered signal, setting the detection threshold at 55% of peak-to-peak amplitude. Self-calibrates within 5–10 seconds of wear — no manual tuning needed.

---

### Step 2 — BPM Calculation

BPM is derived from the **RR interval** (time between consecutive R-peaks):

```
BPM = 60,000 / RR_interval_ms
```

- 500ms refractory period prevents double-triggering
- Exponential moving average smoothing (80/20 weighting)
- Physiological sanity check: 40–200 BPM range enforced

---

### Step 3 — LED Animation

Each detected heartbeat triggers a **lub-dub animation** on the WS2812 ring:

| Phase | Duration | Description |
|-------|----------|-------------|
| Lub (main pulse) | 450ms | Fast attack (12% window) → exponential decay, cyan-white |
| Dub (secondary) | 220ms | Triggered 120ms after main, softer decay, warmer tone |
| Idle glow | Continuous | Dim cyan between beats |
| Lead-off warning | — | Slow red blink when electrodes disconnected |

Animation runs at a locked **50fps** tick independent of the ECG sampling loop.

---

### Step 4 — CTF Web Server

The ESP32S3 broadcasts a **WPA2 WiFi AP** (`CipherCore-MK2`). Once connected, participants access a hacking challenge at `192.168.4.1` — **5 intentionally vulnerable levels**, each gated behind the previous:

| Level | Challenge | Vulnerability Class |
|-------|-----------|-------------------|
| L1 | Find the flag in the API | API Enumeration |
| L2 | Bypass the vault | Broken Access Control |
| L3 | Log in without a password | SQL Injection |
| L4 | Escalate your privileges | JWT Tampering |
| L5 | Break out of the shell | Command Injection |

All five flags must be submitted at `/submit` to unlock the **LED control panel**.

The **proof-of-pwn is physical** — the first person to submit all flags gains live control of the reactor LEDs on the wearer's chest, in front of the entire room.

```
WiFi Crack → L1 → L2 → L3 → L4 → L5 → /submit → REACTOR UNLOCKED
```

---

### Step 5 — Circuit Connections

```
EXG Pill          →    XIAO ESP32S3
─────────────────────────────────────
OUT               →    D10  (ADC input)
LO+               →    D1   (lead-off detection)
LO-               →    D2   (lead-off detection)
VCC               →    3.3V
GND               →    GND

WS2812B Strip     →    XIAO ESP32S3
─────────────────────────────────────
DIN               →    D0
VCC               →    5V (from power bank)
GND               →    GND
```

---

### Step 6 — Assembly

1. Print the arc reactor housing in silver/gray PLA or PETG
2. Route the WS2812 strip around the inner ring channel
3. Mount the EXG Pill and XIAO ESP32S3 inside the housing
4. Attach neodymium magnets to housing back and harness base
5. Run electrode wires under clothing to chest (3-lead Einthoven triangle placement)
6. Power the whole system from a USB power bank via XIAO's USB-C port

---

### Demo Video

> 📹 *[YouTube demo link — reactor pulsing with live heartbeat]*

---

### Code

Full source code available in this repository:

| File | Description |
|------|-------------|
| `src/ciphercore_ctf.ino` | Main firmware — ECG pipeline, LED animation, CTF web server |
| `src/ctf_pages.h` | All CTF HTML pages |
| `platformio.ini` | PlatformIO build config |

---

## 📚 References

1. [Upside Down Labs EXG Pill](https://github.com/upsidedownlabs/BioAmp-EXG-Pill)
2. [Seeed Studio XIAO ESP32S3](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
3. [Adafruit NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel)
4. Pan, J. & Tompkins, W.J. (1985). *A Real-Time QRS Detection Algorithm.* IEEE Transactions on Biomedical Engineering.
5. [OWASP Top 10 Web Application Security Risks](https://owasp.org/www-project-top-ten/)
6. [SO-ARM 101 Robotic Arm](https://github.com/TheRobotStudio/SO-ARM100)

---

## 👤 About the Builder

**c1ph3r** — Robotics Engineer | Hardware Hacker | Cybersecurity Conference Speaker
Presents at DEF CON, BSides, and Seasides | Hardware Hacking Village & Workshop Organizer

---

*Built with ❤️ and a live ECG signal.*
