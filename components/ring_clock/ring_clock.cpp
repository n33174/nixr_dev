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

    for (int i = R1_NUM_LEDS; i < TOTAL_LEDS; i++) {
      if (i % (R2_NUM_LEDS/12) == 0) {
        it[i] = light::ESPColor(100, 100, 100);
      }
    }

  }

  void RingClock::addressable_lights_lambdacall(light::AddressableLight & it) {
    if(_state == state::booting) {
      return clear(it);
    }
    else if(_state == state::time) {
      return render_time(it);
    }
  }

  state RingClock::get_state() {
    return _state;
  }

  void RingClock::set_state(state state) {
    _state = state;
  }

  void RingClock::clear(light::AddressableLight & it) {
    for (int i = 0; i < TOTAL_LEDS; i++) {
      it[i] = light::ESPColor(0, 0, 0);
    }
  }

  void RingClock::render_time(light::AddressableLight & it) {

    clear(it);
    draw_scale(it);
    esphome::ESPTime now = _time->now(); // Corrected namespace for ESPTime

    int hour = now.hour;
    if (hour >= 12) {
      hour = hour - 12;
    }

    it[R1_NUM_LEDS + (hour * 4)] = light::ESPColor(255, 0, 0);
    it[now.minute] = light::ESPColor(0, 255, 0);
    it[now.second] = light::ESPColor(0, 0, 255);

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

} // namespace ring_clock
} // namespace esphome