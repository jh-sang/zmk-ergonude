#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/rgb_underglow.h>

// 이 레포는 Zephyr 모듈이라 이 파일이 settings_reset 등 다른 쉴드 빌드에도 전역 컴파일.
// RGB/indicator가 꺼진 빌드에서는 참조 심볼이 없어 링크가 깨지므로 반드시 가드로 비워둬야 한다.
#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW) && IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)

// HID LED usage 순서: NumLock=bit0, CapsLock=bit1, ScrollLock=bit2
#define HID_INDICATOR_CAPS_LOCK BIT(1)

// 브리딩 한 사이클(밝음→어둠→밝음) 시간. ZMK 순정 breathe는 최대 속도가
// 2.4초로 고정이라 쓰지 않고 여기서 직접 밝기를 굴린다. 빠르기는 이 값만 조절.
#define BREATHE_PERIOD_MS 800
#define BREATHE_TICK_MS 25
#define STEPS_PER_CYCLE (BREATHE_PERIOD_MS / BREATHE_TICK_MS)

static bool breathing;
static uint8_t orig_brt;
static uint32_t step;

static void breathe_tick_cb(struct k_work *work) {
  // 현재 색을 매번 읽으므로 브리딩 중 hue/sat이 바뀌어도(모드 전환 등) 따라간다.
  struct zmk_led_hsb hsb = zmk_rgb_underglow_calc_brt(0);

  // 삼각파: orig_brt → 0 → orig_brt
  uint32_t phase = step % STEPS_PER_CYCLE;
  uint32_t half = STEPS_PER_CYCLE / 2;
  uint32_t tri = phase < half ? (half - phase) : (phase - half);
  hsb.b = (uint8_t)(orig_brt * tri / half);

  zmk_rgb_underglow_set_hsb(hsb);
  step++;
}
static K_WORK_DEFINE(breathe_tick_work, breathe_tick_cb);

static void breathe_timer_cb(struct k_timer *timer) { k_work_submit(&breathe_tick_work); }
static K_TIMER_DEFINE(breathe_timer, breathe_timer_cb, NULL);

// OS가 보내주는 Caps Lock LED 신호(HID indicator)를 기준으로 브리딩을 켜고 끈다.
// 펌웨어 내부 토글 상태를 따로 들고 있지 않으므로 desync가 발생하지 않는다.
static int caps_indicator_rgb_listener(const zmk_event_t *eh) {
  const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
  if (ev == NULL) {
    return ZMK_EV_EVENT_BUBBLE;
  }

  bool caps_on = ev->indicators & HID_INDICATOR_CAPS_LOCK;
  if (caps_on && !breathing) {
    breathing = true;
    zmk_rgb_underglow_select_effect(0); // Solid 위에서 밝기만 굴린다
    orig_brt = zmk_rgb_underglow_calc_brt(0).b;
    step = 0;
    k_timer_start(&breathe_timer, K_NO_WAIT, K_MSEC(BREATHE_TICK_MS));
  } else if (!caps_on && breathing) {
    breathing = false;
    k_timer_stop(&breathe_timer);
    struct k_work_sync sync;
    k_work_cancel_sync(&breathe_tick_work, &sync);
    struct zmk_led_hsb hsb = zmk_rgb_underglow_calc_brt(0);
    hsb.b = orig_brt; // 원래 밝기 복원 (hue/sat은 현재 값 유지)
    zmk_rgb_underglow_set_hsb(hsb);
  }

  return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(caps_indicator_rgb, caps_indicator_rgb_listener);
ZMK_SUBSCRIPTION(caps_indicator_rgb, zmk_hid_indicators_changed);

#endif // CONFIG_ZMK_RGB_UNDERGLOW && CONFIG_ZMK_HID_INDICATORS
