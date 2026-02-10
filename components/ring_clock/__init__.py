import esphome.config_validation as cv
import esphome.codegen as cg
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID
from esphome.components import time as time_, light, switch

DEPENDENCIES = ["network"]

CONF_ON_READY = 'on_ready'
CONF_ON_TIMER_FINISHED = 'on_timer_finished'
CONF_ON_STOPWATCH_MINUTE = 'on_stopwatch_minute'

light_ns = cg.esphome_ns.namespace("light")
LightState = light_ns.class_("LightState", cg.Component)
AddressableLightState = light_ns.class_("LightState", LightState)


# C++ namespace
ns = cg.esphome_ns.namespace("ring_clock")
RingClock = ns.class_("RingClock", cg.Component)
ReadyTrigger = ns.class_('ReadyTrigger', automation.Trigger.template())
TimerFinishedTrigger = ns.class_('TimerFinishedTrigger', automation.Trigger.template())
StopwatchMinuteTrigger = ns.class_('StopwatchMinuteTrigger', automation.Trigger.template())

CONFIG_SCHEMA = cv.Schema({
    #ID
    cv.GenerateID(): cv.declare_id(RingClock),
    # Time
    cv.Required("time_id"): cv.use_id(time_.RealTimeClock),
    # Light Object
    cv.Required("light_id"): cv.use_id(light.AddressableLightState),
    # Custom Value Switches
    cv.Required("enable_seconds"): cv.use_id(switch.Switch),
    cv.Required("enable_scale"): cv.use_id(switch.Switch),
    # Custom Light Values
    cv.Required("hour_hand_color"): cv.use_id(light.LightState),
    cv.Required("minute_hand_color"): cv.use_id(light.LightState),
    cv.Required("second_hand_color"): cv.use_id(light.LightState),
    cv.Required("scale_color"): cv.use_id(light.LightState),
    cv.Required("notification_color"): cv.use_id(light.LightState),

    cv.Optional(CONF_ON_READY): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadyTrigger),
    }),
    cv.Optional(CONF_ON_TIMER_FINISHED): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(TimerFinishedTrigger),
    }),
    cv.Optional(CONF_ON_STOPWATCH_MINUTE): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(StopwatchMinuteTrigger),
    }),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    # Time
    wrapped_time = await cg.get_variable(config["time_id"])
    cg.add(var.set_time(wrapped_time))
    # Light Object
    wrapped_clock_leds = await cg.get_variable(config["light_id"])
    cg.add(var.set_clock_addressable_lights(wrapped_clock_leds))
    #Custom Value Switches
    wrapped_enable_seconds = await cg.get_variable(config["enable_seconds"])
    cg.add(var.set_enable_seconds_state(wrapped_enable_seconds))
    wrapped_enable_scale = await cg.get_variable(config["enable_scale"])
    cg.add(var.set_enable_scale_state(wrapped_enable_scale))
    # Custom Light Values
    wrapped_hour_hand_color = await cg.get_variable(config["hour_hand_color"])
    cg.add(var.set_hour_hand_color_state(wrapped_hour_hand_color))
    wrapped_minute_hand_color = await cg.get_variable(config["minute_hand_color"])
    cg.add(var.set_minute_hand_color_state(wrapped_minute_hand_color))
    wrapped_second_hand_color = await cg.get_variable(config["second_hand_color"])
    cg.add(var.set_second_hand_color_state(wrapped_second_hand_color))
    wrapped_scale_color = await cg.get_variable(config["scale_color"])
    cg.add(var.set_scale_color_state(wrapped_scale_color))
    wrapped_notification_color = await cg.get_variable(config["notification_color"])
    cg.add(var.set_notification_color_state(wrapped_notification_color))

    for conf in config.get(CONF_ON_READY, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)
    for conf in config.get(CONF_ON_TIMER_FINISHED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)
    for conf in config.get(CONF_ON_STOPWATCH_MINUTE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    await cg.register_component(var, config)