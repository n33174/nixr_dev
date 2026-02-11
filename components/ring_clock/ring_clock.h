#pragma once
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include "esphome/components/light/addressable_light.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include <vector>

// Define Ring
#define TOTAL_LEDS 108
#define R1_NUM_LEDS 60
#define R2_NUM_LEDS 48

namespace esphome {
namespace ring_clock {

  // Declare TAG for logging (defined in .cpp)
  extern const char *TAG;

  // Default Colors
  static const Color DEFAULT_COLOR_HOUR(255, 145, 0);         // Brand Orange
  static const Color DEFAULT_COLOR_MINUTE(0, 255, 255);       // Cyan
  static const Color DEFAULT_COLOR_SECOND(200, 200, 200);     // White
  static const Color DEFAULT_COLOR_NOTIFICATION(255, 145, 0); // Brand Orange
  static const Color DEFAULT_COLOR_MARKERS(50, 50, 50);         // Dim White

  enum state
  {
    booting,
    time,
    time_fade,
    time_rainbow,
    timer,
    stopwatch,
    alarm,
    ota,
    shutdown,
    sensors_bars,
    sensors_temp_glow,
    sensors_humid_glow,
    sensors_ticks,
    sensors_dual_glow,
    sensors_temp_bar,
    sensors_humid_bar,
    sensors_dual_bar,
    sensors_temp_tick,
    sensors_humid_tick,
    sensors_dual_tick
  };

  class RingClock : public Component {
    public:
      // Method declarations
      void setup() override;
      void loop() override;
      void add_on_ready_callback(std::function<void()> callback);
      void on_ready();
      void draw_markers(light::AddressableLight & it);
      void addressable_lights_lambdacall(light::AddressableLight & it);
      state get_state();
      void set_state(state state);
      void clear_R1(light::AddressableLight & it);
      void clear_R2(light::AddressableLight & it);
      void render_time(light::AddressableLight & it, bool fade);
      void render_rainbow(light::AddressableLight & it);
      float get_interference_factor();
      void set_time(time::RealTimeClock *time);
      void set_clock_addressable_lights(light::LightState *it);
      void set_hour_hand_color_state(light::LightState* state);
      void set_minute_hand_color_state(light::LightState* state);
      void set_enable_seconds_state(switch_::Switch *enable_seconds);
      void set_second_hand_color_state(light::LightState* state);
      void set_enable_markers_state(switch_::Switch *enable_markers);
      void set_marker_color_state(light::LightState* state);
      void set_notification_color_state(light::LightState* state);
      void set_blank_leds(std::vector<int> leds);
      
      // Timer and Stopwatch controls
      void start_timer(int hours, int minutes, int seconds);
      void stop_timer();
      void start_stopwatch();
      void stop_stopwatch();
      void reset_stopwatch();
      void add_on_timer_finished_callback(std::function<void()> callback);
      void on_timer_finished();
      void add_on_stopwatch_minute_callback(std::function<void()> callback);
      void on_stopwatch_minute();
      
      void add_on_timer_started_callback(std::function<void()> callback);
      void on_timer_started();
      void add_on_timer_stopped_callback(std::function<void()> callback);
      void on_timer_stopped();
      void add_on_stopwatch_started_callback(std::function<void()> callback);
      void on_stopwatch_started();
      void add_on_stopwatch_paused_callback(std::function<void()> callback);
      void on_stopwatch_paused();
      void add_on_stopwatch_reset_callback(std::function<void()> callback);
      void on_stopwatch_reset();

      // Alarm controls
      void start_alarm();
      void add_on_alarm_triggered_callback(std::function<void()> callback);
      void on_alarm_triggered();

      void set_sound_enabled_state(switch_::Switch *sound_enabled) { this->_sound_enabled_switch = sound_enabled; }
      void set_temperature_sensor(sensor::Sensor *temp) { this->_temp_sensor = temp; }
      void set_humidity_sensor(sensor::Sensor *humid) { this->_humidity_sensor = humid; }

      
      // Default color setters
      void set_default_hour_color(Color color) { _default_hour_color = color; }
      void set_default_minute_color(Color color) { _default_minute_color = color; }
      void set_default_second_color(Color color) { _default_second_color = color; }
      void set_default_notification_color(Color color) { _default_notification_color = color; }
      void set_default_marker_color(Color color) { _default_marker_color = color; }

    protected:
      std::vector<int> _blanked_leds;
      CallbackManager<void()> _on_ready_callback_;
      bool _has_time{false};
      float _interference_factor{0.0f};
      state _state{state::booting};
      light::LightState *_clock_lights;
      time::RealTimeClock *_time;
      int last_second{-1};
      uint32_t last_second_timestamp{0};
      light::LightState* hour_hand_color{nullptr};
      light::LightState* minute_hand_color{nullptr};
      switch_::Switch* enable_seconds{nullptr};
      light::LightState* second_hand_color{nullptr};
      switch_::Switch* enable_markers{nullptr};
      light::LightState* marker_color{nullptr};
      light::LightState* notification_color{nullptr};

      void render_timer(light::AddressableLight & it);
      void render_stopwatch(light::AddressableLight & it);
      void render_alarm(light::AddressableLight & it);
      void render_sensors_bars(light::AddressableLight & it);
      void render_sensors_ticks(light::AddressableLight & it);
      void render_sensors_temp_glow(light::AddressableLight & it);
      void render_sensors_humid_glow(light::AddressableLight & it);
      void render_sensors_dual_glow(light::AddressableLight & it);
      void render_sensors_bar_individual(light::AddressableLight & it, bool is_temp);
      void render_sensors_tick_individual(light::AddressableLight & it, bool is_temp);

      Color get_temp_color(float t);
      Color get_humid_color(float h);

      // Timer and Stopwatch state
      bool _timer_active{false};
      uint32_t _timer_target_ms{0};
      uint32_t _timer_duration_ms{0};
      
      bool _stopwatch_active{false};
      uint32_t _stopwatch_start_ms{0};
      uint32_t _stopwatch_paused_ms{0};

      CallbackManager<void()> _on_timer_finished_callback_;
      uint32_t _timer_finished_ms{0};
      bool _timer_finishing_dispatched{false};
      
      CallbackManager<void()> _on_stopwatch_minute_callback_;
      int _stopwatch_last_minute{-1};

      CallbackManager<void()> _on_timer_started_callback_;
      CallbackManager<void()> _on_timer_stopped_callback_;
      CallbackManager<void()> _on_stopwatch_started_callback_;
      CallbackManager<void()> _on_stopwatch_paused_callback_;
      CallbackManager<void()> _on_stopwatch_reset_callback_;

      // Alarm state
      CallbackManager<void()> _on_alarm_triggered_callback_;
      uint32_t _alarm_triggered_ms{0};
      bool _alarm_dispatched{false};
      bool _alarm_active{false};

      switch_::Switch* _sound_enabled_switch{nullptr};
      sensor::Sensor* _temp_sensor{nullptr};
      sensor::Sensor* _humidity_sensor{nullptr};


      // Default colors (Initialized from constants above)
      Color _default_hour_color = DEFAULT_COLOR_HOUR;
      Color _default_minute_color = DEFAULT_COLOR_MINUTE;
      Color _default_second_color = DEFAULT_COLOR_SECOND;
      Color _default_notification_color = DEFAULT_COLOR_NOTIFICATION;
      Color _default_marker_color = DEFAULT_COLOR_MARKERS;
  };

  class ReadyTrigger : public Trigger<> {
    public:
      explicit ReadyTrigger(RingClock *parent);
  };

  class TimerFinishedTrigger : public Trigger<> {
    public:
      explicit TimerFinishedTrigger(RingClock *parent);
  };

  class StopwatchMinuteTrigger : public Trigger<> {
    public:
      explicit StopwatchMinuteTrigger(RingClock *parent);
  };

  class AlarmTriggeredTrigger : public Trigger<> {
    public:
      explicit AlarmTriggeredTrigger(RingClock *parent);
  };

  class TimerStartedTrigger : public Trigger<> {
    public:
      explicit TimerStartedTrigger(RingClock *parent);
  };
  class TimerStoppedTrigger : public Trigger<> {
    public:
      explicit TimerStoppedTrigger(RingClock *parent);
  };
  class StopwatchStartedTrigger : public Trigger<> {
    public:
      explicit StopwatchStartedTrigger(RingClock *parent);
  };
  class StopwatchPausedTrigger : public Trigger<> {
    public:
      explicit StopwatchPausedTrigger(RingClock *parent);
  };
  class StopwatchResetTrigger : public Trigger<> {
    public:
      explicit StopwatchResetTrigger(RingClock *parent);
  };

} // namespace ring_clock
} // namespace esphome