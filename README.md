# NIX labs AL60 Clock Firmware

This repository contains the ESPHome firmware for the NIX labs AL60 Clock, an advanced dual-ring WS2812 LED clock powered by an ESP32-C3.

## Repository Structure

- **[al60.yaml](file:///Users/nathan/Documents/GitHub/nixr_dev/al60.yaml)**: Main entry point for standard users (pulls remote packages).
- **[al60_factory.yaml](file:///Users/nathan/Documents/GitHub/nixr_dev/al60_factory.yaml)**: Entry point for development/factory use (uses local packages).
- **[packages/](file:///Users/nathan/Documents/GitHub/nixr_dev/packages/)**: Modular configuration blocks:
  - `al60_core.yaml`: System setup, ESP32-C3 performance tuning.
  - `al60_light.yaml`: LED effects and dummy controls for UI.
  - `al60_inputs.yaml`: Physical buttons and user interaction logic.
  - `al60_sensors.yaml`: Hardware sensors (AHT20, LDR, occupancy).
  - `al60_time.yaml`: RTC and SNTP time synchronization.
  - `al60_radar.yaml`: Optional UART integration for LD2410.
- **[components/ring_clock/](file:///Users/nathan/Documents/GitHub/nixr_dev/components/ring_clock/)**: Custom C++ component driving the clock rendering.

## Using the `ring_clock` Component in Other Projects

The `ring_clock` component can be reused in other ESPHome projects that use a similar dual-ring LED layout (60 inner, 48 outer).

### Prerequisites

- An ESP32 (ESP32-C3 recommended).
- A 60-LED inner ring (Minutes/Seconds).
- A 48-LED outer ring (Hours/Markers).
- An external Real Time Clock (e.g., PCF8563).

### YAML Configuration

Add the component to your ESPHome config:

```yaml
external_components:
  - source: github://n33174/nixr_dev@main
    components: [ring_clock]

ring_clock:
  id: my_clock
  time_id: rtc_time_component
  light_id: ring_lights_component
  hour_sweep_switch: sweep_switch
  hour_hand_color: hour_color_light
  minute_hand_color: minute_color_light
  second_hand_color: second_color_light
  marker_color: marker_color_light
  notification_color: notif_color_light
  sound_enabled_switch: sound_switch
```

## YAML Customization

If you have purchased a NIX labs AL60 Clock, you can customize its behavior by editing `al60.yaml`.

### 1. Core Settings

Modify the `substitutions` block at the top of the file:

- `name`: Network hostname.
- `friendly_name`: Display name in Home Assistant.
- `time_zone`: Fallback POSIX timezone string.

### 2. Calibration Offsets

In `packages/al60_sensors.yaml`, you can adjust:

- `temp_offset_default`: Calibration for internal heat (default `-8°C`).
- `occupancy_cooloff_default`: Seconds before turning off display when no motion is detected.

### 3. Enabling Optional Features

To enable full UART data from the LD2410 radar sensor, uncomment the `radar` package line in `al60.yaml`.

## License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**.

You are free to use, modify, and distribute the code and precompiled firmware. Any distributed derivative works (including compiled binaries) must also be released under the GPLv3.
