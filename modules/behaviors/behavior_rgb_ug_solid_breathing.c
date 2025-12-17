#include <zephyr/kernel.h>
#include <zmk/behavior.h>


struct zmk_rgb_underglow_state {
    u8_t current_effect;
};

extern struct zmk_rgb_underglow_state state;
extern void zmk_rgb_underglow_select_effect(u8_t effect);


#define DT_DRV_COMPAT zmk_behavior_rgb_ug_solid_breathing

// Behavior 정의 구조체
struct behavior_rgb_ug_solid_breathing_config {};
struct behavior_rgb_ug_solid_breathing_data {};

// ZMK Underglow Effect 인덱스
const u8_t SOLID_MODE_INDEX = 0; 
const u8_t BREATHING_MODE_INDEX = 1; 

// behavior_rgb_ug_solid_breathing_init 함수
static int behavior_rgb_ug_solid_breathing_init(const struct device *dev) {
    return 0;
}


static int behavior_rgb_ug_solid_breathing_press(struct zmk_behavior_binding *binding,
                                               struct zmk_behavior_binding_event event) {

    if (state.current_effect == SOLID_MODE_INDEX) {
        zmk_rgb_underglow_select_effect(BREATHING_MODE_INDEX);
    } else {
        zmk_rgb_underglow_select_effect(SOLID_MODE_INDEX);
    }

    return ZMK_BEHAVIOR_OPAQUE;
}


static int behavior_rgb_ug_solid_breathing_release(struct zmk_behavior_binding *binding,
                                                 struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

// Behavior API 정의
static const struct zmk_behavior_api behavior_rgb_ug_solid_breathing_api = {
    .press = behavior_rgb_ug_solid_breathing_press,
    .release = behavior_rgb_ug_solid_breathing_release,
};

// Behavior 디바이스 정의
ZMK_BEHAVIOR_DT_DEFINE(behavior_rgb_ug_solid_breathing, behavior_rgb_ug_solid_breathing_init,
                       NULL, behavior_rgb_ug_solid_breathing_data,
                       behavior_rgb_ug_solid_breathing_config, POST_KERNEL,
                       CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_rgb_ug_solid_breathing_api);

// Behavior DT 호환성 정의
#define ZMK_BEHAVIOR_RGB_UG_SOLID_BREATHING_COMPAT zmk_behavior_rgb_ug_solid_breathing
