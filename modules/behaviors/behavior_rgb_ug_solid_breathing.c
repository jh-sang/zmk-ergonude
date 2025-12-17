#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/rgb_underglow.h>
#include <zmk/endpoints.h>

/* 최신 ZMK Underglow 효과 인덱스 (보통 순서상 SOLID=0, BREATH=1 인 경우가 많으나 환경에 따라 다를 수 있음) */
#define EFFECT_SOLID 0
#define EFFECT_BREATHING 1

static int behavior_rgb_ug_solid_breathing_binding_pressed(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    uint8_t current_effect;
    
    // 1. 현재 Underglow의 효과 번호를 가져옵니다.
    // 최신 ZMK에서는 zmk_rgb_underglow_get_state()를 통해 상태를 확인할 수 있습니다.
    int ret = zmk_rgb_underglow_get_state(&current_effect);
    if (ret != 0) return ret;

    // 2. 토글 로직: 현재가 Solid면 Breathing으로, 아니면 Solid로 변경
    if (current_effect == EFFECT_SOLID) {
        return zmk_rgb_underglow_select_effect(EFFECT_BREATHING);
    } else {
        return zmk_rgb_underglow_select_effect(EFFECT_SOLID);
    }
}

static int behavior_rgb_ug_solid_breathing_binding_released(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    return 0;
}

static const struct zmk_behavior_api behavior_rgb_ug_solid_breathing_api = {
    .on_key_param_pressed = behavior_rgb_ug_solid_breathing_binding_pressed,
    .on_key_param_released = behavior_rgb_ug_solid_breathing_binding_released,
};

// 장치 초기화 매크로 (기존 소스 하단부 유지)
#define RGB_UG_SOLID_BREATHING_INST(n) \
    DEVICE_DT_INST_DEFINE(n, NULL, NULL, \
                          NULL, NULL, \
                          APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, \
                          &behavior_rgb_ug_solid_breathing_api);

DT_INST_FOREACH_STATUS_OKAY(RGB_UG_SOLID_BREATHING_INST)
