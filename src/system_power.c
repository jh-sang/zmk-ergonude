#include <zephyr/kernel.h>
#include <zmk/behavior.h>
#include <zmk/hid.h>
#include <zmk/endpoints.h>

#define REPORT_ID_SYSTEM 3
#define USAGE_SYSTEM_POWER_DOWN 0x81

static int system_power_pressed(struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event) {
    uint8_t report[2] = {USAGE_SYSTEM_POWER_DOWN, 0x00};

    zmk_hid_send_report(REPORT_ID_SYSTEM, report, sizeof(report));
    return ZMK_BEHAVIOR_OPAQUE;
}

static int system_power_released(struct zmk_behavior_binding *binding,
                                 struct zmk_behavior_binding_event event) {
    uint8_t report[2] = {0x00, 0x00};

    zmk_hid_send_report(REPORT_ID_SYSTEM, report, sizeof(report));
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api system_power_driver_api = {
    .binding_pressed = system_power_pressed,
    .binding_released = system_power_released,
};

BEHAVIOR_DEFINE(system_power, behavior_system_power, system_power_driver_api);
