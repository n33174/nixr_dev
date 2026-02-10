#pragma once
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include "esphome/components/light/addressable_light.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/switch/switch.h"
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
  static const Color DEFAULT_COLOR_SECOND(255, 255, 255);     // White
  static const Color DEFAULT_COLOR_NOTIFICATION(255, 145, 0); // Brand Orange
  static const Color DEFAULT_COLOR_SCALE(50, 50, 50);         // Dim White

  enum state
  {
    booting,
    time,
    time_fade,
    time_rainbow,
    timer,
    stopwatch,
    ota,
    shutdown
  };

  class RingClock : public Component {
    public:
      // Method declarations
      void setup() override;
      void loop() override;
      void add_on_ready_callback(std::function<void()> callback);
      void on_ready();
      void draw_scale(light::AddressableLight & it);
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
      void set_enable_scale_state(switch_::Switch *enable_scale);
      void set_scale_color_state(light::LightState* state);
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
      void set_sound_enabled_state(switch_::Switch *sound_enabled) { this->_sound_enabled_switch = sound_enabled; }

      
      // Default color setters
      void set_default_hour_color(Color color) { _default_hour_color = color; }
      void set_default_minute_color(Color color) { _default_minute_color = color; }
      void set_default_second_color(Color color) { _default_second_color = color; }
      void set_default_notification_color(Color color) { _default_notification_color = color; }
      void set_default_scale_color(Color color) { _default_scale_color = color; }

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
      switch_::Switch* enable_scale{nullptr};
      light::LightState* scale_color{nullptr};
      light::LightState* notification_color{nullptr};

      void render_timer(light::AddressableLight & it);
      void render_stopwatch(light::AddressableLight & it);

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
      switch_::Switch* _sound_enabled_switch{nullptr};


      // Default colors (Initialized from constants above)
      Color _default_hour_color = DEFAULT_COLOR_HOUR;
      Color _default_minute_color = DEFAULT_COLOR_MINUTE;
      Color _default_second_color = DEFAULT_COLOR_SECOND;
      Color _default_notification_color = DEFAULT_COLOR_NOTIFICATION;
      Color _default_scale_color = DEFAULT_COLOR_SCALE;
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

} // namespace ring_clock
} // namespace esphome