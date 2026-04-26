# Comprehensive Test Plan - AL60 Clock

This test plan is designed to verify all core functionality, edge cases, and potential regressions before product release.

## 1. Core Time & Sync Verification

### 1.1 First Boot / Factory State
* **Test**: Erase all settings and boot the device.
* **Expected**:
  * Fallback AP `NIX labs AL60 Clock` starts (Password: `12345678`).
  * Web UI is accessible at `192.168.4.1`.
  * Time defaults to a sensible fallback (e.g., 2026-01-01).

### 1.2 Internet Time Sync (SNTP)
* **Test**: Connect to WiFi with internet access.
* **Expected**:
  * Device connects and syncs time automatically.
  * Timezone is auto-detected based on IP.
  * Hardware RTC is updated with correct time.

### 1.3 Offline Operation
* **Test**: Reboot device without internet access (or block NTP).
* **Expected**:
  * Device boots and restores time from Hardware RTC.
  * Clock continues to run accurately.

### 1.4 Manual Time Entry
* **Test**: Set time via Web UI "Custom Time" picker.
* **Expected**:
  * Time updates immediately on the clock face.
  * SNTP Sync is automatically disabled.
  * Setting survives a reboot.

## 2. Visual Modes & Effects

### 2.1 Physical Button Control (Mode Button)
* **Test**: Click, Double Click, Long Press.
* **Expected**:
  * **Single Click**: Cycles through Clock styles (Standard, Fade, Tail, Rainbow Tail, RGB, Mono).
  * **Double Click**: Cycles Sensor Effects on background (Bars, Glow, Ticks).
  * **Long Press**: Cycles Light Control Mode (Motion, Ambient, Both, Manual).

### 2.2 Brightness Management
* **Test**: Change Light Control Mode.
* **Expected**:
  * **Manual**: HA Slider controls brightness directly.
  * **Ambient**: Brightness adjusts smoothly as room light changes.
  * **Motion**: Dims when no movement is detected (Cool-off time respected).

## 3. Edge Cases & Robustness

### 3.1 Button Chords (Maintenance)
* **Test**: Hold Hour + Minute buttons simultaneously.
* **Expected**:
  * **2 Seconds**: Reboot warning sound plays.
  * **Release at 3s**: Reboots with SNTP re-enabled.
  * **Hold 10s**: Plays reset sound and deletes all configuration.

### 3.2 Fast Button Presses
* **Test**: Rapidly click physical buttons.
* **Expected**:
  * System remains responsive; no crashes or missed steps.

### 3.3 Sensor Failure
* **Test**: Disconnect AHT20 sensor (if hardware permits) or simulate I2C failure.
* **Expected**:
  * System falls back gracefully; no boot loops.
