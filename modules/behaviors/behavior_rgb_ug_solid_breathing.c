#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/rgb_underglow.h>

/* [빌드 에러 방지] ZMK 최신 API 구조체 직접 선언 */
struct zmk_behavior_api {
    int (*on_key_param_pressed)(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event);
    int (*on_key_param_released)(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event);
};

/* 효과 번호 (ZMK 표준: 0은 Solid, 1은 Breathe) */
#define EFFECT_SOLID 0
#define EFFECT_BREATHING 1

static int behavior_rgb_ug_solid_breathing_binding_pressed(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    LOG_ERR("HB_RGB KEY PRESSED!");
    // 내부에서 상태를 기억합니다. 
    // 키보드를 처음 켰을 때와 싱크가 안 맞으면 한 번 더 누르면 됩니다!
    static bool is_breathing = false;
    return zmk_rgb_underglow_select_effect(1); /*
    if (is_breathing) {
        is_breathing = false;
        return zmk_rgb_underglow_select_effect(EFFECT_SOLID);
    } else {
        is_breathing = true;
        return zmk_rgb_underglow_select_effect(EFFECT_BREATHING);
    }
    */
}

static int behavior_rgb_ug_solid_breathing_binding_released(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    return 0; // 뗐을 때는 아무 동작 안 함
}

static const struct zmk_behavior_api behavior_rgb_ug_solid_breathing_api = {
    .on_key_param_pressed = behavior_rgb_ug_solid_breathing_binding_pressed,
    .on_key_param_released = behavior_rgb_ug_solid_breathing_binding_released,
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
