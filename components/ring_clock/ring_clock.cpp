#include "ring_clock.h"

namespace esphome {
namespace ring_clock {

  // Define TAG
  const char *TAG = "ring_clock.component";

  // RingClock method implementations
  void RingClock::setup() {
  }

  void RingClock::loop(){
    if(!_has_time && _time->now().is_valid()) {
      _has_time=true;
      on_ready();
      return;
    }
  }

  void RingClock::add_on_ready_callback(std::function<void()> callback) {
    this->_on_ready_callback_.add(std::move(callback));
  }

  void RingClock::on_ready() {
    _state = state::time;
    ESP_LOGD(TAG, "ready");
    this->_on_ready_callback_.call();
  }

  void RingClock::draw_scale(light::AddressableLight & it) {

    if (this->notification_color != nullptr && this->notification_color->current_values.get_state()) {
      auto color_values = this->notification_color->current_values;
      // Get the brightness from the light component (0.0 to 1.0)
      float brightness = color_values.get_brightness();
      // Scale the RGB values by the brightness
      uint8_t r = static_cast<uint8_t>(color_values.get_red() * 255 * brightness);
      uint8_t g = static_cast<uint8_t>(color_values.get_green() * 255 * brightness);
      uint8_t b = static_cast<uint8_t>(color_values.get_blue() * 255 * brightness);
      //Inner Ring Color
      for (int i = R1_NUM_LEDS; i < TOTAL_LEDS; i++) {
        it[i] = Color(r, g, b);
      }
    } else {
      // Not Lit
    }

    if (this->enable_scale != nullptr) {
      if (this->enable_scale->state) {
        if (this->scale_color != nullptr && this->scale_color->current_values.get_state()) {
          auto color_values = this->scale_color->current_values;
          // Get the brightness from the light component (0.0 to 1.0)
          float brightness = color_values.get_brightness();
          // Scale the RGB values by the brightness
          uint8_t r = static_cast<uint8_t>(color_values.get_red() * 255 * brightness);
          uint8_t g = static_cast<uint8_t>(color_values.get_green() * 255 * brightness);
          uint8_t b = static_cast<uint8_t>(color_values.get_blue() * 255 * brightness);
          // Set LEDs
          for (int i = R1_NUM_LEDS; i < TOTAL_LEDS; i++) {
            if (i % (R2_NUM_LEDS/12) == 0) {
              it[i] = Color(r, g, b);
            }
          }
        } else {
          // Set Default Scale Color
          for (int i = R1_NUM_LEDS; i < TOTAL_LEDS; i++) {
            if (i % (R2_NUM_LEDS/12) == 0) {
              it[i] = Color(255, 255, 255);
            }
          }
        }
      }
    }

  }

  void RingClock::addressable_lights_lambdacall(light::AddressableLight & it) {
    if(_state == state::booting) {
      clear_R1(it);
      clear_R2(it);
      it[0] = Color(0, 255, 0); //Green
    } else if(_state == state::shutdown) {
      clear_R1(it);
      clear_R2(it);
      it[0] = Color(255, 0, 0); //Red
    } else if(_state == state::ota) {
      clear_R1(it);
      clear_R2(it);
      it[0] = Color(0, 0, 255); //Blue
    } else if(_state == state::time) {
      return render_time(it, false);
    } else if(_state == state::time_fade) {
      return render_time(it, true);
    }
  }

  state RingClock::get_state() {
    return _state;
  }

  void RingClock::set_state(state state) {
    _state = state;
  }

  void RingClock::clear_R1(light::AddressableLight & it) {
    for (int i = 0; i < R1_NUM_LEDS; i++) {
      it[i] = Color(0, 0, 0);
    }
  }

  void RingClock::clear_R2(light::AddressableLight & it) {
    for (int i = R1_NUM_LEDS; i < TOTAL_LEDS; i++) {
      it[i] = Color(0, 0, 0);
    }
  }

  void RingClock::render_time(light::AddressableLight & it, bool fade) {

    clear_R1(it);
    clear_R2(it);
    draw_scale(it);
    esphome::ESPTime now = _time->now(); // Corrected namespace for ESPTime

    int hour = now.hour;
    if (hour >= 12) {
      hour = hour - 12;
    }


    if (this->hour_hand_color != nullptr && this->hour_hand_color->current_values.get_state()) {
      auto color_values = this->hour_hand_color->current_values;
      // Get the brightness from the light component (0.0 to 1.0)
      float brightness = color_values.get_brightness();
      // Scale the RGB values by the brightness
      uint8_t r = static_cast<uint8_t>(color_values.get_red() * 255 * brightness);
      uint8_t g = static_cast<uint8_t>(color_values.get_green() * 255 * brightness);
      uint8_t b = static_cast<uint8_t>(color_values.get_blue() * 255 * brightness);
      //Hour Color
      it[R1_NUM_LEDS + (hour * 4)] = Color(r, g, b);
    } else {
      // Hour Default Red
      it[R1_NUM_LEDS + (hour * 4)] = Color(255, 0, 0);
    }

    if (this->enable_seconds != nullptr) {
      if (this->enable_seconds->state) {
        Color second_color = Color(0, 0, 255); // Default Blue

        if (this->second_hand_color != nullptr && this->second_hand_color->current_values.get_state()) {
          auto color_values = this->second_hand_color->current_values;
          float brightness = color_values.get_brightness();
          uint8_t r = static_cast<uint8_t>(color_values.get_red() * 255 * brightness);
          uint8_t g = static_cast<uint8_t>(color_values.get_green() * 255 * brightness);
          uint8_t b = static_cast<uint8_t>(color_values.get_blue() * 255 * brightness);
          second_color = Color(r, g, b);
        }

        if (fade) {
            // Smooth fade with spread (3 LEDs lit)
            
            // 1. Synchronize monotonic millis with integer seconds to prevent jumping
            if (this->last_second != now.second) {
                this->last_second = now.second;
                this->last_second_timestamp = millis();
            }

            long dt = millis() - this->last_second_timestamp;
            float progress = dt / 1000.0f;
            // Clamp to avoid overshooting if clock tick is late
            if (progress > 1.0f) progress = 1.0f;
            
            float precise_pos = now.second + progress;
            
            // 2. Iterate all LEDs to find neighbors (handling circular wrap)
            for (int i = 0; i < 60; i++) {
                float dist = abs(i - precise_pos);
                // Handle circular distance (e.g. distance between 0 and 59 is 1)
                if (dist > 30.0f) dist = 60.0f - dist;
                
                // Radius of 1.5 allows for 3 LEDs to be lit (Center + 1 neighbor on each side usually)
                // dist 0 -> 100%, dist 1.5 -> 0%
                if (dist < 1.5f) {
                    float brightness_scale = 1.0f - (dist / 1.5f);
                    
                    Color c = Color(
                        (uint8_t)(second_color.r * brightness_scale),
                        (uint8_t)(second_color.g * brightness_scale),
                        (uint8_t)(second_color.b * brightness_scale)
                    );
                    it[i] = c;
                }
            }
            
        } else {
            // Standard step Logic
            it[now.second] = second_color;
        }
      }
    }

    if (this->minute_hand_color != nullptr && this->minute_hand_color->current_values.get_state()) {
      auto color_values = this->minute_hand_color->current_values;
      // Get the brightness from the light component (0.0 to 1.0)
      float brightness = color_values.get_brightness();
      // Scale the RGB values by the brightness
      uint8_t r = static_cast<uint8_t>(color_values.get_red() * 255 * brightness);
      uint8_t g = static_cast<uint8_t>(color_values.get_green() * 255 * brightness);
      uint8_t b = static_cast<uint8_t>(color_values.get_blue() * 255 * brightness);
      //Minute Color
      it[now.minute] = Color(r, g, b);
    } else {
      // Minute Default Green
      it[now.minute] = Color(0, 255, 0);
    }

  }

  void RingClock::set_time(time::RealTimeClock *time) {
    _time = time;
  }

  void RingClock::set_clock_addressable_lights(light::LightState *it) {
    _clock_lights = it;
  }

  // ReadyTrigger constructor implementation
  ReadyTrigger::ReadyTrigger(RingClock *parent) { // Corrected type
    parent->add_on_ready_callback([this]() { this->trigger(); });
  }

  void RingClock::set_hour_hand_color_state(light::LightState* state) {
    this->hour_hand_color = state;
  }

  void RingClock::set_minute_hand_color_state(light::LightState* state) {
    this->minute_hand_color = state;
  }

  void RingClock::set_enable_seconds_state(switch_::Switch *enable_seconds) {
      this->enable_seconds = enable_seconds;
  }

  void RingClock::set_second_hand_color_state(light::LightState* state) {
    this->second_hand_color = state;
  }

  void RingClock::set_enable_scale_state(switch_::Switch *enable_scale) {
      this->enable_scale = enable_scale;
  }

  void RingClock::set_scale_color_state(light::LightState* state) {
    this->scale_color = state;
  }

  void RingClock::set_notification_color_state(light::LightState* state) {
    this->notification_color = state;
  }

} // namespace ring_clock
} // namespace esphome