# 🐭 MicroMouse-Pico2-2026

An autonomous maze-solving robot built using **Raspberry Pi Pico 2**.  
Designed for high-speed navigation using the **Flood Fill Algorithm**.

---

## 🚀 Project in Action

### 🧭 Simulation Discovery Phase



The robot explores the maze, builds a distance map using Flood Fill, and calculates the shortest path to the center.

---

## ✨ Key Features

- ⚡ Fast flood-fill maze solving algorithm
- 🧠 Intelligent path optimization
- 🔄 Multi-run learning capability
- 📊 Encoder-based motion control
- 📡 Wall detection using IR sensors

---

## 🛠 Hardware Specification

## 🛠 Hardware Components

| Hardware Components | Details |
|---------------------|----------|
| Microcontroller | Raspberry Pi Pico 2 |
| Motor Driver | MX1616H |
| Motors | N20 motors with encoder (2 motors) |
| Sensors | VL6180X TOF sensors (front, left, right) |
| Power | 7.4V Li-ion Battery |

---

## 🔌 Connection Table

## 🔌 Pin Configuration

### 🧭 Sensor Connections

| Component | Pico GPIO Pin |
|------------|---------------|
| Front Sensor | GP19 |
| Right Sensor | GP20 |
| Left Sensor | GP18 |

---

### 🔘 Push Buttons

| Button | Pico GPIO Pin |
|--------|---------------|
| Right Button | GP21 |
| Left Button | GP22 |

---

### 🔊 Buzzer

| Component | Pico GPIO Pin |
|------------|---------------|
| Buzzer | GP17 |

---

### ⚙ Motor Driver Connections

#### Left Motor

| Function | Pico GPIO Pin |
|----------|---------------|
| M1 | GP14 |
| M2 | GP15 |

#### Right Motor

| Function | Pico GPIO Pin |
|----------|---------------|
| M1 | GP12 |
| M2 | GP13 |

---

### 🎯 Encoder Connections

#### Left Encoder

| Channel | Pico GPIO Pin |
|----------|---------------|
| A | GP8 |
| B | GP9 |

#### Right Encoder

| Channel | Pico GPIO Pin |
|----------|---------------|
| A | GP10 |
| B | GP11 |
---

## 🧠 Algorithm

This project uses the **Flood Fill Algorithm**:

1. Initialize maze with default high values
2. Set goal cell to 0
3. Propagate values outward
4. Always move to lowest neighbor cell
5. Repeat until goal reached

---

## 📂 Project Structure
