
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <dt-bindings/zmk/rgb.h>
#include <zmk/rgb_underglow.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(behavior_rgb_ug_solid_breathing, CONFIG_ZMK_LOG_LEVEL);

#define DT_DRV_COMPAT zmk_behavior_rgb_ug_solid_breathing

// 초기화 함수
static int behavior_rgb_ug_solid_breathing_init(const struct device *dev) {
    return 0;
}

static int behavior_rgb_ug_solid_breathing_binding_pressed(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    LOG_ERR("HB_RGB KEY PRESSED!");

    return zmk_rgb_underglow_select_effect(1); /*
    static bool is_breathing = false;
    if (is_breathing) {
        is_breathing = false;
        return zmk_rgb_underglow_select_effect(0);
    } else {
        is_breathing = true;
        return zmk_rgb_underglow_select_effect(1);
    }
    */
}

static int behavior_rgb_ug_solid_breathing_binding_released(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    return 0; // 뗐을 때는 아무 동작 안 함
}

static const struct behavior_driver_api behavior_rgb_ug_solid_breathing_driver_api = {
    .binding_pressed = behavior_rgb_ug_solid_breathing_binding_pressed,
    .binding_released = behavior_rgb_ug_solid_breathing_binding_released,
};

// 인스턴스 생성
#define RGB_UG_INST(n) \
    BEHAVIOR_DT_INST_DEFINE(n, \
                            behavior_rgb_ug_solid_breathing_init, \
                            NULL, \
                            NULL, \
                            NULL, \
                            POST_KERNEL, \
                            CONFIG_ZMK_BEHAVIOR_INIT_PRIORITY, \
                            &behavior_rgb_ug_solid_breathing_driver_api);

DT_INST_FOREACH_STATUS_OKAY(RGB_UG_INST)

