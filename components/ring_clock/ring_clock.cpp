#include "ring_clock.h"

namespace esphome {
namespace ring_clock {

  // Define TAG for logging
  const char *TAG = "ring_clock.component";

  // --- Lifecycle ---

  void RingClock::setup() {
    // Standard setup hooks if needed
  }

  void RingClock::loop(){
    // Check if time has become valid (synced via NTP or RTC)
    if(!_has_time && _time->now().is_valid()) {
      _has_time = true;
      on_ready(); // Trigger ready event (enables clock display)
      return;
    }

    // Handle Alarm Timeout
    if (_alarm_active) {
      // Trigger the automation once
      if (!_alarm_dispatched) {
        this->on_alarm_triggered();
        _alarm_dispatched = true;
      }

      // Auto-dismiss visual alarm after 10 seconds
      if (millis() - _alarm_triggered_ms > 10000) {
        _alarm_active = false;
      }
    }
  }

  // --- Event & Callback Handlers ---

  void RingClock::on_ready() {
    _state = state::time;
    ESP_LOGD(TAG, "RingClock Ready: Time is valid.");
    this->_on_ready_callback_.call();
  }

  void RingClock::add_on_ready_callback(std::function<void()> callback) {
    this->_on_ready_callback_.add(std::move(callback));
  }

  // Timer Events
  void RingClock::add_on_timer_finished_callback(std::function<void()> callback) { this->_on_timer_finished_callback_.add(std::move(callback)); }
  void RingClock::on_timer_finished() {
    // Only fire event if sound is enabled (or switch not configured)
    if (this->_sound_enabled_switch == nullptr || this->_sound_enabled_switch->state) {
      this->_on_timer_finished_callback_.call();
    }
  }

  void RingClock::add_on_timer_started_callback(std::function<void()> callback) { this->_on_timer_started_callback_.add(std::move(callback)); }
  void RingClock::on_timer_started() { 
      if (this->_sound_enabled_switch == nullptr || this->_sound_enabled_switch->state) this->_on_timer_started_callback_.call(); 
  }

  void RingClock::add_on_timer_stopped_callback(std::function<void()> callback) { this->_on_timer_stopped_callback_.add(std::move(callback)); }
  void RingClock::on_timer_stopped() { 
      if (this->_sound_enabled_switch == nullptr || this->_sound_enabled_switch->state) this->_on_timer_stopped_callback_.call(); 
  }

  // Stopwatch Events
  void RingClock::add_on_stopwatch_minute_callback(std::function<void()> callback) { this->_on_stopwatch_minute_callback_.add(std::move(callback)); }
  void RingClock::on_stopwatch_minute() {
    if (this->_sound_enabled_switch == nullptr || this->_sound_enabled_switch->state) {
      this->_on_stopwatch_minute_callback_.call();
    }
  }

  void RingClock::add_on_stopwatch_started_callback(std::function<void()> callback) { this->_on_stopwatch_started_callback_.add(std::move(callback)); }
  void RingClock::on_stopwatch_started() { 
      if (this->_sound_enabled_switch == nullptr || this->_sound_enabled_switch->state) this->_on_stopwatch_started_callback_.call(); 
  }

  void RingClock::add_on_stopwatch_paused_callback(std::function<void()> callback) { this->_on_stopwatch_paused_callback_.add(std::move(callback)); }
  void RingClock::on_stopwatch_paused() { 
      if (this->_sound_enabled_switch == nullptr || this->_sound_enabled_switch->state) this->_on_stopwatch_paused_callback_.call(); 
  }

  void RingClock::add_on_stopwatch_reset_callback(std::function<void()> callback) { this->_on_stopwatch_reset_callback_.add(std::move(callback)); }
  void RingClock::on_stopwatch_reset() { 
      if (this->_sound_enabled_switch == nullptr || this->_sound_enabled_switch->state) this->_on_stopwatch_reset_callback_.call(); 
  }

  // Alarm Events
  void RingClock::add_on_alarm_triggered_callback(std::function<void()> callback) { this->_on_alarm_triggered_callback_.add(std::move(callback)); }
  void RingClock::on_alarm_triggered() { this->_on_alarm_triggered_callback_.call(); }
  
  void RingClock::start_alarm() {
    _alarm_triggered_ms = millis();
    _alarm_dispatched = false;
    _alarm_active = true;
  }

  // --- Logic Control ---

  void RingClock::start_timer(int hours, int minutes, int seconds) {
    // Limit to max 12h 59m 59s
    if (hours > 12) hours = 12;
    if (hours == 12 && minutes > 59) minutes = 59;
    if (hours == 12 && minutes == 59 && seconds > 59) seconds = 59;

    _timer_duration_ms = (hours * 3600 + minutes * 60 + seconds) * 1000;
    _timer_target_ms = millis() + _timer_duration_ms;
    _timer_active = true;
    _timer_finished_ms = 0;
    _timer_finishing_dispatched = false;
    
    _state = state::timer;
    this->on_timer_started();
  }

  void RingClock::stop_timer() {
    _timer_active = false;
    this->on_timer_stopped();
  }

  void RingClock::start_stopwatch() {
    if (!_stopwatch_active) {
      // Resume from pause or start new
      _stopwatch_start_ms = millis() - _stopwatch_paused_ms;
      _stopwatch_active = true;
      _stopwatch_last_minute = -1;
      this->on_stopwatch_started();
    }
    _state = state::stopwatch;
  }

  void RingClock::stop_stopwatch() {
    if (_stopwatch_active) {
      // Pause
      _stopwatch_paused_ms = millis() - _stopwatch_start_ms;
      _stopwatch_active = false;
      this->on_stopwatch_paused();
    }
  }

  void RingClock::reset_stopwatch() {
    _stopwatch_start_ms = millis();
    _stopwatch_paused_ms = 0;
    _stopwatch_last_minute = -1;
    this->on_stopwatch_reset();
  }

  state RingClock::get_state() { return _state; }
  void RingClock::set_state(state state) { _state = state; }

  // --- Configuration Setters ---

  void RingClock::set_time(time::RealTimeClock *time) { _time = time; }
  void RingClock::set_hour_hand_color_state(light::LightState* state) { this->hour_hand_color = state; }
  void RingClock::set_minute_hand_color_state(light::LightState* state) { this->minute_hand_color = state; }
  void RingClock::set_enable_seconds_state(switch_::Switch *enable_seconds) { this->enable_seconds = enable_seconds; }
  void RingClock::set_second_hand_color_state(light::LightState* state) { this->second_hand_color = state; }
  void RingClock::set_enable_markers_state(switch_::Switch *enable_markers) { this->enable_markers = enable_markers; }
  void RingClock::set_marker_color_state(light::LightState* state) { this->marker_color = state; }
  void RingClock::set_notification_color_state(light::LightState* state) { this->notification_color = state; }
  void RingClock::set_clock_addressable_lights(light::LightState *it) { this->_clock_lights = it; }
  void RingClock::set_blank_leds(std::vector<int> leds) { this->_blanked_leds = leds; }
  float RingClock::get_interference_factor() { return this->_interference_factor; }

  // --- Helpers ---

  void RingClock::clear_R1(light::AddressableLight & it) {
    for (int i = 0; i < R1_NUM_LEDS; i++) it[i] = Color(0, 0, 0);
  }

  void RingClock::clear_R2(light::AddressableLight & it) {
    for (int i = R1_NUM_LEDS; i < TOTAL_LEDS; i++) it[i] = Color(0, 0, 0);
  }

  // --- Rendering Logic ---

  void RingClock::addressable_lights_lambdacall(light::AddressableLight & it) {
    // Dispatch to specific render function based on state
    switch(_state) {
      case state::booting:
        clear_R1(it); clear_R2(it);
        it[0] = Color(255, 145, 0); // Orange indicator for boot
        break;
      case state::shutdown:
        clear_R1(it); clear_R2(it);
        it[0] = Color(255, 0, 0); // Red
        break;
      case state::time:
        render_time(it, false);
        break;
      case state::time_fade:
        render_time(it, true);
        break;
      case state::time_rainbow:
        render_rainbow(it);
        break;
      case state::timer:
        render_timer(it);
        break;
      case state::stopwatch:
        render_stopwatch(it);
        break;
      case state::sensors_bars:
        render_sensors_bars(it);
        break;
      case state::sensors_temp_bar:
        render_sensors_bar_individual(it, true);
        break;
      case state::sensors_humid_bar:
        render_sensors_bar_individual(it, false);
        break;
      case state::sensors_temp_glow:
        render_sensors_temp_glow(it);
        break;
      case state::sensors_humid_glow:
        render_sensors_humid_glow(it);
        break;
      case state::sensors_dual_glow:
        render_sensors_dual_glow(it);
        break;
      case state::sensors_ticks:
        render_sensors_ticks(it);
        break;
      case state::sensors_temp_tick:
        render_sensors_tick_individual(it, true);
        break;
      case state::sensors_humid_tick:
        render_sensors_tick_individual(it, false);
        break;
    }
    
    // Overlay: Alarm Animation
    if (_alarm_active) {
      this->render_alarm(it);
    }
    
    // Overlay: Blank LEDs (hardware masking)
    if (!_blanked_leds.empty()) {
      for (int led_index : _blanked_leds) {
        if (led_index >= 0 && led_index < TOTAL_LEDS) {
          it[led_index] = Color(0, 0, 0);
        }
      }
    }

    // Calc Interference for Light Sensor
    // We average LEDs 15 and 72 because they are physically physically closest to the light sensor
    if (TOTAL_LEDS > 72) {
      auto c15 = it[15].get(); // R1 (Inner) LED 15
      auto c72 = it[72].get(); // R2 (Outer) LED 12 (60+12)
      float b15 = (c15.r + c15.g + c15.b) / 3.0f;
      float b72 = (c72.r + c72.g + c72.b) / 3.0f;
      this->_interference_factor = (b15 + b72) / 255.0f;
    }
  }

  void RingClock::draw_markers(light::AddressableLight & it) {
    // Draws static markers on the Outer Ring (R2)
    // Check if we are running a specialized sensor effect that replaces markers
    bool sensor_effect_active = false;

    if (this->notification_color != nullptr) {
      std::string effect = this->notification_color->get_effect_name();
      
      // If a sensor effect is active in the "Notification" layer, delegate to it
      if (effect.find("Sensors:") != std::string::npos) {
          // This complex if-chain dispatches the exact sensor effect found in the string
          if (effect == "Sensors: Dual Bars") { this->render_sensors_bars(it); sensor_effect_active = true; } 
          else if (effect == "Sensors: Temperature Bar") { this->render_sensors_bar_individual(it, true); sensor_effect_active = true; } 
          else if (effect == "Sensors: Humidity Bar") { this->render_sensors_bar_individual(it, false); sensor_effect_active = true; } 
          else if (effect == "Sensors: Temperature Glow") { this->render_sensors_temp_glow(it); sensor_effect_active = true; } 
          else if (effect == "Sensors: Humidity Glow") { this->render_sensors_humid_glow(it); sensor_effect_active = true; } 
          else if (effect == "Sensors: Dual Ticks") { this->render_sensors_ticks(it); sensor_effect_active = true; } 
          else if (effect == "Sensors: Temperature Tick") { this->render_sensors_tick_individual(it, true); sensor_effect_active = true; } 
          else if (effect == "Sensors: Humidity Tick") { this->render_sensors_tick_individual(it, false); sensor_effect_active = true; } 
          else if (effect == "Sensors: Dual Glow") { this->render_sensors_dual_glow(it); sensor_effect_active = true; }
      }
    }

    // If no sensor effect overrides the background:
    if (!sensor_effect_active) {
      // 1. Draw "Notification" Color Background (if enabled)
      if (this->notification_color != nullptr && this->notification_color->current_values.get_state()) {
        auto cv = this->notification_color->current_values;
        float b = cv.get_brightness();
        uint8_t r = (uint8_t)(cv.get_red() * 255 * b);
        uint8_t g = (uint8_t)(cv.get_green() * 255 * b);
        uint8_t blue = (uint8_t)(cv.get_blue() * 255 * b);
        
        // Minimum brightness clamp for visibility
        if (cv.get_red() > 0 && r < 20) r = 20;
        if (cv.get_green() > 0 && g < 20) g = 20;
        if (cv.get_blue() > 0 && blue < 20) blue = 20;

        Color bg = Color(r, g, blue);

        // Fill Outer Ring
        for (int i = R1_NUM_LEDS; i < TOTAL_LEDS; i++) {
          // If markers enabled, don't overwrite marker positions (every 4th LED)
          if (this->enable_markers != nullptr && this->enable_markers->state) {
              if ((i - R1_NUM_LEDS) % 4 != 0) it[i] = bg;
          } else {
              it[i] = bg;
          }
        }
      }

      // 2. Draw Markers (every 4th LED on Outer Ring)
      if (this->enable_markers != nullptr && this->enable_markers->state) {
        Color mc = _default_marker_color;
        
        // Use custom marker color if set
        if (this->marker_color != nullptr && this->marker_color->current_values.get_state()) {
           auto cv = this->marker_color->current_values;
           float b = cv.get_brightness();
           mc = Color((uint8_t)(cv.get_red() * 255 * b), 
                      (uint8_t)(cv.get_green() * 255 * b), 
                      (uint8_t)(cv.get_blue() * 255 * b));
        }

        for (int i = R1_NUM_LEDS; i < TOTAL_LEDS; i++) {
          if ((i - R1_NUM_LEDS) % 4 == 0) {
            it[i] = mc;
          }
        }
      }
    }
  }

  void RingClock::render_time(light::AddressableLight & it, bool fade) {
    clear_R1(it);
    clear_R2(it);
    draw_markers(it);
    
    esphome::ESPTime now = _time->now();

    // --- Hour Hand (Outer Ring R2, 12 positions mapped to 48 LEDs) ---
    int hour = now.hour % 12;
    Color hc = _default_hour_color;
    
    if (this->hour_hand_color != nullptr && this->hour_hand_color->current_values.get_state()) {
       auto cv = this->hour_hand_color->current_values;
       float b = cv.get_brightness();
       hc = Color((uint8_t)(cv.get_red() * 255 * b), (uint8_t)(cv.get_green() * 255 * b), (uint8_t)(cv.get_blue() * 255 * b));
    }
    // R2 starts at index 60. Each hour gets a 4-LED block.
    it[R1_NUM_LEDS + (hour * 4)] = hc;

    // --- Seconds Hand (Inner Ring R1, 60 mapped to 60) ---
    if (this->enable_seconds != nullptr && this->enable_seconds->state) {
      Color sc = _default_second_color;
      if (this->second_hand_color != nullptr && this->second_hand_color->current_values.get_state()) {
         auto cv = this->second_hand_color->current_values;
         float b = cv.get_brightness();
         sc = Color((uint8_t)(cv.get_red() * 255 * b), (uint8_t)(cv.get_green() * 255 * b), (uint8_t)(cv.get_blue() * 255 * b));
      }

      if (fade) {
        // Smooth interpolation
        if (this->last_second != now.second) {
          this->last_second = now.second;
          this->last_second_timestamp = millis();
        }
        
        float progress = (millis() - this->last_second_timestamp) / 1000.0f;
        if (progress > 1.0f) progress = 1.0f;
        
        float precise_pos = now.second + progress;
        
        // Render anti-aliased pixel
        for (int i = 0; i < 60; i++) {
          float dist = abs(i - precise_pos);
          if (dist > 30.0f) dist = 60.0f - dist; // Wrap around
          
          // 1.5 LED width for smoothness
          if (dist < 1.5f) {
            float brightness_scale = 1.0f - (dist / 1.5f);
            it[i] = Color((uint8_t)(sc.r * brightness_scale), (uint8_t)(sc.g * brightness_scale), (uint8_t)(sc.b * brightness_scale));
          }
        }
      } else {
        it[now.second] = sc;
      }
    }

    // --- Minute Hand (Inner Ring R1, 60 mapped to 60) ---
    Color mc_color = _default_minute_color;
    if (this->minute_hand_color != nullptr && this->minute_hand_color->current_values.get_state()) {
       auto cv = this->minute_hand_color->current_values;
       float b = cv.get_brightness();
       mc_color = Color((uint8_t)(cv.get_red() * 255 * b), (uint8_t)(cv.get_green() * 255 * b), (uint8_t)(cv.get_blue() * 255 * b));
    }
    it[now.minute] = mc_color;
  }

  void RingClock::render_rainbow(light::AddressableLight & it) {
    clear_R2(it);
    draw_markers(it);
    
    esphome::ESPTime now = _time->now();
    int hour = now.hour % 12;

    // --- Hour Hand ---
    Color hc = _default_hour_color;
    if (this->hour_hand_color != nullptr && this->hour_hand_color->current_values.get_state()) {
       auto cv = this->hour_hand_color->current_values;
       float b = cv.get_brightness();
       hc = Color((uint8_t)(cv.get_red() * 255 * b), (uint8_t)(cv.get_green() * 255 * b), (uint8_t)(cv.get_blue() * 255 * b));
    }
    it[R1_NUM_LEDS + (hour * 4)] = hc;

    // --- Rainbow Seconds Tail ---
    if (this->enable_seconds != nullptr && this->enable_seconds->state) {
      if (this->last_second != now.second) {
        this->last_second = now.second;
        this->last_second_timestamp = millis();
      }
      float progress = (millis() - this->last_second_timestamp) / 1000.0f;
      if (progress > 1.0f) progress = 1.0f;
      float precise_pos = now.second + progress;
      
      // Hue offset shifts the rainbow based on the minute, making it dynamic
      float hue_offset = now.minute * (360.0f/60.0f); 
      int tail_length = 40;

      for (int i = 0; i < 60; i++) {
        float dist = precise_pos - i;
        if (dist < 0) dist += 60.0f; // Wrap around logic

        if (dist >= 0 && dist < tail_length) {
          // Fade out intensity further back in the tail
          float tail_intensity = 1.0f - (dist / (float)tail_length);
          tail_intensity = tail_intensity * tail_intensity; // Non-linear fade
          
          float hue = (i * (360.0f / 60.0f)) + hue_offset; 
          float r_f, g_f, b_f;
          esphome::hsv_to_rgb(hue, 1.0f, 1.0f, r_f, g_f, b_f);
          it[i] = Color((uint8_t)(r_f * 255.0f * tail_intensity), (uint8_t)(g_f * 255.0f * tail_intensity), (uint8_t)(b_f * 255.0f * tail_intensity));
        }
      }
    }
    
    // --- Minute Hand ---
    Color mc = _default_minute_color;
    if (this->minute_hand_color != nullptr && this->minute_hand_color->current_values.get_state()) {
       auto cv = this->minute_hand_color->current_values;
       float b = cv.get_brightness();
       mc = Color((uint8_t)(cv.get_red() * 255 * b), (uint8_t)(cv.get_green() * 255 * b), (uint8_t)(cv.get_blue() * 255 * b));
    }
    it[now.minute] = mc;
  }

  // --- Utility: Colors from Values ---

  Color RingClock::get_temp_color(float t) {
    // Gradient map: Cold (Blue-ish) -> Comfortable (Green) -> Hot (Red)
    if (t <= -10.0f) return Color(26, 22, 73);
    if (t <= 0.0f) {
        float p = (t + 10.0f) / 10.0f;
        return Color((uint8_t)(26 + p * (18-26)), (uint8_t)(22 + p * (94-22)), (uint8_t)(73 + p * (131-73)));
    }
    if (t <= 10.0f) {
        float p = t / 10.0f;
        return Color((uint8_t)(18 + p * (60-18)), (uint8_t)(94 + p * (157-94)), (uint8_t)(131 + p * (116-131)));
    }
    if (t <= 20.0f) {
        float p = (t - 10.0f) / 10.0f;
        return Color((uint8_t)(60 + p * (207-60)), (uint8_t)(157 + p * (216-157)), (uint8_t)(116 + p * (113-116))); // Good
    }
    if (t <= 30.0f) {
        float p = (t - 20.0f) / 10.0f;
        return Color((uint8_t)(207 + p * (223-207)), (uint8_t)(216 + p * (158-216)), (uint8_t)(113 + p * (60-113)));
    }
    if (t <= 40.0f) {
        float p = (t - 30.0f) / 10.0f;
        return Color((uint8_t)(223 + p * (193-223)), (uint8_t)(158 + p * (59-158)), (uint8_t)(60 + p * (44-60)));
    }
    if (t <= 50.0f) {
        float p = (t - 40.0f) / 10.0f;
        return Color((uint8_t)(193 + p * (136-193)), (uint8_t)(59 + p * (26-59)), (uint8_t)(44 + p * (23-44)));
    }
    return Color(136, 26, 23); // Very Hot
  }

  Color RingClock::get_humid_color(float h) {
    // Gradient map: Dry (Brown/Yellow) -> Comfortable (Green) -> Wet (Blue)
    if (h <= 30.0f) {
        float p = h / 30.0f;
        return Color((uint8_t)(190 - p * 30), (uint8_t)(155 + p * 40), (uint8_t)(47 - p * 20)); // Dry
    } else if (h <= 70.0f) {
        float p = (h - 30.0f) / 40.0f;
        return Color((uint8_t)(160 - p * 100), (uint8_t)(195 + p * 20), (uint8_t)(27 + p * 100)); // Good
    } else {
        float p = (h - 70.0f) / 30.0f;
        if (p > 1.0f) p = 1.0f;
        return Color((uint8_t)(60 - p * 40), (uint8_t)(215 - p * 120), (uint8_t)(127 + p * 100)); // Wet
    }
  }

  void RingClock::render_sensors_bars(light::AddressableLight & it) {
    float temp = _temp_sensor ? _temp_sensor->state : 20.0f;
    float humid = _humidity_sensor ? _humidity_sensor->state : 50.0f;

    // 1. Temperature Bar (Right Side of R2: Hours 0-5)
    // Map -10C to 50C -> 0 to 18 LEDs (3 per hour * 6 hours)
    float t_p = (temp + 10.0f) / 60.0f;
    int t_leds = (int)(t_p * 18.0f);
    if (t_leds < 0) t_leds = 0; if (t_leds > 18) t_leds = 18;

    int count = 0;
    // Iterate clockwise down (5 -> 0)
    for (int h = 5; h >= 0; h--) {
        for (int s = 3; s >= 1; s--) { // Fill outer edge in
            if (count < t_leds) {
                float val = -10.0f + (count * 3.33f);
                it[R1_NUM_LEDS + (h * 4) + s] = get_temp_color(val);
            }
            count++;
        }
    }

    // 2. Humidity Bar (Left Side of R2: Hours 6-11)
    // Map 0% to 100% -> 0 to 18 LEDs
    float h_p = humid / 100.0f;
    int h_leds = (int)(h_p * 18.0f);
    if (h_leds < 0) h_leds = 0; if (h_leds > 18) h_leds = 18;

    count = 0;
    // Iterate clockwise up (6 -> 11)
    for (int h = 6; h <= 11; h++) {
        for (int s = 1; s <= 3; s++) { // Fill inner edge out
            if (count < h_leds) {
                float val = count * 5.55f;
                 it[R1_NUM_LEDS + (h * 4) + s] = get_humid_color(val);
            }
            count++;
        }
    }
  }

  void RingClock::render_sensors_temp_glow(light::AddressableLight & it) {
    float temp = _temp_sensor ? _temp_sensor->state : 20.0f;
    Color glow = get_temp_color(temp);
    // Fill all markers/spaces with glow color
    for (int i = 0; i < 12; i++) {
        it[R1_NUM_LEDS + (i * 4) + 1] = glow;
        it[R1_NUM_LEDS + (i * 4) + 2] = glow;
        it[R1_NUM_LEDS + (i * 4) + 3] = glow;
    }
  }

  void RingClock::render_sensors_humid_glow(light::AddressableLight & it) {
    float humid = _humidity_sensor ? _humidity_sensor->state : 50.0f;
    Color glow = get_humid_color(humid);
    for (int i = 0; i < 12; i++) {
        it[R1_NUM_LEDS + (i * 4) + 1] = glow;
        it[R1_NUM_LEDS + (i * 4) + 2] = glow;
        it[R1_NUM_LEDS + (i * 4) + 3] = glow;
    }
  }

  void RingClock::render_sensors_ticks(light::AddressableLight & it) {
    float temp = _temp_sensor ? _temp_sensor->state : 20.0f;
    float humid = _humidity_sensor ? _humidity_sensor->state : 50.0f;

    // 1. Temperature Tick (Right Side)
    float t_p = (temp + 10.0f) / 60.0f;
    if (t_p < 0) t_p = 0; if (t_p > 1.0f) t_p = 1.0f;
    int t_led_idx = (int)(t_p * 17.99f);

    int count = 0;
    for (int h = 5; h >= 0; h--) {
        for (int s = 3; s >= 1; s--) {
            if (count == t_led_idx) {
                float val = -10.0f + (count * 3.33f);
                it[R1_NUM_LEDS + (h * 4) + s] = get_temp_color(val);
            }
            count++;
        }
    }

    // 2. Humidity Tick (Left Side)
    float h_p = humid / 100.0f;
    if (h_p < 0) h_p = 0; if (h_p > 1.0f) h_p = 1.0f;
    int h_led_idx = (int)(h_p * 17.99f);

    count = 0;
    for (int h = 6; h <= 11; h++) {
        for (int s = 1; s <= 3; s++) {
            if (count == h_led_idx) {
                float val = count * 5.55f;
                it[R1_NUM_LEDS + (h * 4) + s] = get_humid_color(val);
            }
            count++;
        }
    }
  }

  void RingClock::render_sensors_tick_individual(light::AddressableLight & it, bool is_temp) {
    // Renders a single tick for either temp or humidity over the whole dial (360 degrees)
    float val = is_temp ? (_temp_sensor ? _temp_sensor->state : 20.0f) : (_humidity_sensor ? _humidity_sensor->state : 50.0f);
    float p = is_temp ? (val + 10.0f) / 60.0f : val / 100.0f;
    if (p < 0) p = 0; if (p > 1.0f) p = 1.0;
    
    // 36 positions (12 hours * 3 slots between markers)
    int led_idx = (int)(p * 35.99f);

    int count = 0;
    for (int h = 0; h < 12; h++) {
        for (int s = 1; s <= 3; s++) {
            if (count == led_idx) {
                it[R1_NUM_LEDS + (h * 4) + s] = is_temp ? get_temp_color(val) : get_humid_color(val);
            }
            count++;
        }
    }
  }

  void RingClock::render_sensors_bar_individual(light::AddressableLight & it, bool is_temp) {
    // Renders a full bar for either temp or humidity over the whole dial
    float val = is_temp ? (_temp_sensor ? _temp_sensor->state : 20.0f) : (_humidity_sensor ? _humidity_sensor->state : 50.0f);
    float p = is_temp ? (val + 10.0f) / 60.0f : val / 100.0f;
    
    int leds = (int)(p * 36.0f);
    if (leds < 0) leds = 0; if (leds > 36) leds = 36;

    int count = 0;
    for (int h = 0; h < 12; h++) {
        for (int s = 1; s <= 3; s++) {
            if (count < leds) {
                float current_val = is_temp ? (-10.0f + (count * (60.0f/36.0f))) : (count * (100.0f/36.0f));
                it[R1_NUM_LEDS + (h * 4) + s] = is_temp ? get_temp_color(current_val) : get_humid_color(current_val);
            }
            count++;
        }
    }
  }

  void RingClock::render_sensors_dual_glow(light::AddressableLight & it) {
    float temp = _temp_sensor ? _temp_sensor->state : 20.0f;
    float humid = _humidity_sensor ? _humidity_sensor->state : 50.0f;

    Color t_color = get_temp_color(temp);
    Color h_color = get_humid_color(humid);

    // Apply Temperature color to Right Side (Hours 0-5)
    for (int h = 0; h <= 5; h++) {
        for (int s = 1; s <= 3; s++) {
            it[R1_NUM_LEDS + (h * 4) + s] = t_color;
        }
    }

    // Apply Humidity color to Left Side (Hours 6-11)
    for (int h = 6; h <= 11; h++) {
        for (int s = 1; s <= 3; s++) {
            it[R1_NUM_LEDS + (h * 4) + s] = h_color;
        }
    }
  }

  void RingClock::render_timer(light::AddressableLight & it) {
    // Shows the remaining time on the clock face
    // Hours on Outer Ring Markers
    // Minutes on Inner Ring
    // Seconds on Inner Ring (Priority)
    
    clear_R1(it); clear_R2(it);

    if (!_timer_active) return;

    long remaining_ms = (long)_timer_target_ms - (long)millis();
    if (remaining_ms < 0) remaining_ms = 0;

    int total_seconds = remaining_ms / 1000;
    int hours = total_seconds / 3600;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;

    auto get_color = [&](light::LightState* state, Color def) {
      if (state != nullptr && state->current_values.get_state()) {
        auto cv = state->current_values;
        float b = cv.get_brightness();
        return Color((uint8_t)(cv.get_red() * 255 * b), (uint8_t)(cv.get_green() * 255 * b), (uint8_t)(cv.get_blue() * 255 * b));
      }
      return def;
    };

    if (total_seconds > 0 && total_seconds < 60) {
      // Less than 1 minute: Fill seconds up to current
      Color sc = get_color(this->second_hand_color, _default_second_color);
      for (int i = 0; i < seconds; i++) it[i] = sc;
    } else if (total_seconds > 0) {
      // Show hours on R2 markers
      Color hc = get_color(this->hour_hand_color, _default_hour_color);
      for (int i = 0; i < 12; i++) {
        if (i < hours) it[R1_NUM_LEDS + (i * 4)] = hc;
      }
      // Show minutes on R1
      Color mc = get_color(this->minute_hand_color, _default_minute_color);
      for (int i = 0; i < minutes; i++) it[i] = mc;
      
      // Show single second pixel
      it[seconds] = get_color(this->second_hand_color, _default_second_color);
    }
    
    // Timer Finished Animation
    if (total_seconds == 0 && _timer_active) {
        if (_timer_finished_ms == 0) _timer_finished_ms = millis();
        
        // Dispatch sound once
        if (!_timer_finishing_dispatched) {
          this->on_timer_finished();
          _timer_finishing_dispatched = true;
        }

        // Pulsing animation
        float pulse = 0.3f + 0.7f * ((sinf(millis() * 0.003f) + 1.0f) / 2.0f);
        Color nc = get_color(this->notification_color, _default_notification_color);
        Color pc = Color((uint8_t)(nc.r * pulse), (uint8_t)(nc.g * pulse), (uint8_t)(nc.b * pulse));
        for (int i = 0; i < 12; i++) {
          int base = R1_NUM_LEDS + (i * 4);
          it[base + 1] = pc; it[base + 2] = pc; it[base + 3] = pc;
        }
    }
  }

  void RingClock::render_stopwatch(light::AddressableLight & it) {
    clear_R1(it); clear_R2(it);

    uint32_t elapsed_ms = _stopwatch_active ? (millis() - _stopwatch_start_ms) : _stopwatch_paused_ms;
    
    // Cap at 11h 59m 59s
    if (elapsed_ms >= 12 * 3600 * 1000) elapsed_ms = 12 * 3600 * 1000 - 1;

    int total_seconds = elapsed_ms / 1000;
    int hours = total_seconds / 3600;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;

    // Minute Chime Logic
    if (minutes != _stopwatch_last_minute && _stopwatch_active) {
      if (_stopwatch_last_minute != -1) this->on_stopwatch_minute();
      _stopwatch_last_minute = minutes;
    }

    auto get_color = [&](light::LightState* state, Color def) {
      if (state != nullptr && state->current_values.get_state()) {
        auto cv = state->current_values;
        float b = cv.get_brightness();
        return Color((uint8_t)(cv.get_red() * 255 * b), (uint8_t)(cv.get_green() * 255 * b), (uint8_t)(cv.get_blue() * 255 * b));
      }
      return def;
    };

    // Render Hands
    Color hc = get_color(this->hour_hand_color, _default_hour_color);
    for (int i = 0; i < 12; i++) {
      if (i < hours) it[R1_NUM_LEDS + (i * 4)] = hc;
    }
    
    Color mc = get_color(this->minute_hand_color, _default_minute_color);
    for (int i = 0; i < minutes; i++) it[i] = mc;
    
    it[seconds] = get_color(this->second_hand_color, _default_second_color);
  }

  void RingClock::render_alarm(light::AddressableLight & it) {
    auto get_color = [&](light::LightState* state, Color def) {
      if (state != nullptr && state->current_values.get_state()) {
        auto cv = state->current_values;
        float b = cv.get_brightness();
        return Color((uint8_t)(cv.get_red() * 255 * b), (uint8_t)(cv.get_green() * 255 * b), (uint8_t)(cv.get_blue() * 255 * b));
      }
      return def;
    };

    // Pulsing Alarm Overlay
    float pulse = 0.3f + 0.7f * ((sinf(millis() * 0.003f) + 1.0f) / 2.0f);
    Color nc = get_color(this->notification_color, _default_notification_color);
    Color pc = Color((uint8_t)(nc.r * pulse), (uint8_t)(nc.g * pulse), (uint8_t)(nc.b * pulse));
    
    // Fill gaps between markers on Outer Ring
    for (int i = 0; i < 12; i++) {
      int base = R1_NUM_LEDS + (i * 4);
      it[base + 1] = pc; it[base + 2] = pc; it[base + 3] = pc;
    }
  }

  // --- Trigger Constructors ---
  ReadyTrigger::ReadyTrigger(RingClock *parent) { parent->add_on_ready_callback([this]() { this->trigger(); }); }
  TimerFinishedTrigger::TimerFinishedTrigger(RingClock *parent) { parent->add_on_timer_finished_callback([this]() { this->trigger(); }); }
  StopwatchMinuteTrigger::StopwatchMinuteTrigger(RingClock *parent) { parent->add_on_stopwatch_minute_callback([this]() { this->trigger(); }); }
  AlarmTriggeredTrigger::AlarmTriggeredTrigger(RingClock *parent) { parent->add_on_alarm_triggered_callback([this]() { this->trigger(); }); }
  TimerStartedTrigger::TimerStartedTrigger(RingClock *parent) { parent->add_on_timer_started_callback([this]() { this->trigger(); }); }
  TimerStoppedTrigger::TimerStoppedTrigger(RingClock *parent) { parent->add_on_timer_stopped_callback([this]() { this->trigger(); }); }
  StopwatchStartedTrigger::StopwatchStartedTrigger(RingClock *parent) { parent->add_on_stopwatch_started_callback([this]() { this->trigger(); }); }
  StopwatchPausedTrigger::StopwatchPausedTrigger(RingClock *parent) { parent->add_on_stopwatch_paused_callback([this]() { this->trigger(); }); }
  StopwatchResetTrigger::StopwatchResetTrigger(RingClock *parent) { parent->add_on_stopwatch_reset_callback([this]() { this->trigger(); }); }

} // namespace ring_clock
} // namespace esphome