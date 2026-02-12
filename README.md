# 🕰️ NIX labs AL60 Clock

The AL60 is a high-precision, ESP32-C3 powered ring clock designed by NIX labs. It features environmental sensing, touch inputs, and seamless Home Assistant integration via ESPHome.



## 🚀 Getting Started

### 1. Web Installer (Easiest)
If you have a NIX labs AL60 hardware unit, you can install or update the firmware directly from your browser:
👉 **[Visit the NIX labs Web Installer](https://n33174.github.io/nixr_dev/)**

### 2. DIY / ESPHome Integration
If you are building your own version or want to customize your NIX labs clock:

1. Add `al60.yaml` to your ESPHome dashboard.
2. The configuration will automatically pull the required packages and custom components from this repository.

---

## 🛠️ Hardware Specifications
* **Core:** ESP32-C3 (RISC-V)
* **Display:** 60x WS2812B Addressable LEDs
* **Framework:** ESP-IDF
* **Features:** WiFi, BLE (Improv), OTA Updates, Web UI

---

## 🔧 Repository Structure
* `/packages`: Modular ESPHome YAML files (Core, Sensors, Light, etc.)
* `/components`: Custom C++ code for the `ring_clock` display logic.
* `al60.yaml`: The public doorway for customer imports.
* `al60_factory.yaml`: The master build file used for production releases.

---

## 🤝 Contributing & DIY
We love seeing improvements! 
* **Building a DIY version?** You are welcome to use our code for personal, non-commercial use.
* **Made an improvement?** Please submit a Pull Request! Under the terms of our license, improvements must be shared back with the community.

---

## 📜 License & Usage

This project is licensed under a split-license model to support the DIY community while protecting the commercial interests of **NIX labs**.

### 🛠️ Software Engine (GPLv3)
The custom C++ components located in the `/components` directory are licensed under the **GNU General Public License v3.0**. This ensures compatibility with the ESPHome ecosystem and guarantees that improvements to the core driver logic remain open.

### 🧩 Configuration & Product Logic (CC BY-NC-SA 4.0)
The ESPHome YAML configurations (`al60.yaml`, `al60_factory.yaml`, and the `/packages` directory), documentation, and creative assets are licensed under **Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International**.

**What this means for you:**
* **DIY Builders:** You are free (and encouraged!) to build, modify, and use this code for your personal projects.
* **Improvements:** If you improve the YAML logic or features, you must share those changes under the same license.
* **Commercial Use:** Sale of hardware or software using these configurations, or use for any profit-generating purpose, is **strictly prohibited** without express written permission from NIX labs.

---

## 📞 Support
Created by **NIX labs**. If you encounter issues with your hardware or firmware, please [Open an Issue](https://github.com/n33174/nixr_dev/issues).