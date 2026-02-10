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

  enum state
  {
    booting,
    time,
    time_fade,
    time_rainbow,
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
  };

  class ReadyTrigger : public Trigger<> {
    public:
      // Corrected constructor parameter type
      explicit ReadyTrigger(RingClock *parent);
  };


} // namespace ring_clock
} // namespace esphome