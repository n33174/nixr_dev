#pragma once
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include "esphome/components/light/addressable_light.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/core/log.h"

// Define Ring
#define TOTAL_LEDS 108
#define R1_NUM_LEDS 60
#define R2_NUM_LEDS 48



// Define macros
#define NUM_CHARS 4
#define NUM_LEDS_PER_SEGMENT 9
#define NUM_SEGMENTS_PER_CHAR 7
#define NUM_LEDS_PER_CHAR (NUM_LEDS_PER_SEGMENT * NUM_SEGMENTS_PER_CHAR)
#define NUM_LEDS_TOTAL (NUM_LEDS_PER_CHAR * NUM_CHARS)

#define MATRIX_WIDTH 80
#define MATRIX_HEIGHT 21

namespace esphome {
namespace ring_clock {

  // Declare TAG for logging (defined in .cpp)
  extern const char *TAG;

  enum state
  {
    booting,
    time,
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
      void clear(light::AddressableLight & it);
      void render_time(light::AddressableLight & it);
      void set_time(time::RealTimeClock *time);
      void set_clock_addressable_lights(light::LightState *it);

    protected:
      CallbackManager<void()> _on_ready_callback_;
      bool _has_time{false};
      state _state{state::booting};
      light::LightState *_clock_lights;
      time::RealTimeClock *_time;
  };

  class ReadyTrigger : public Trigger<> {
    public:
      // Corrected constructor parameter type
      explicit ReadyTrigger(RingClock *parent);
  };


} // namespace ring_clock
} // namespace esphome