#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define P0_05_PORT DT_NODELABEL(gpio0)
#define P0_05_PIN  5

static const struct device *gpio0_dev;
static struct k_work_delayable fix_work;

// 配置P0.05为强下拉输入
static int configure_p0_05_strong_pull(void)
{
    int ret;
    
    if (!gpio0_dev) {
        gpio0_dev = DEVICE_DT_GET(P0_05_PORT);
    }
    
    if (!device_is_ready(gpio0_dev)) {
        LOG_ERR("GPIO0 device not ready");
        return -ENODEV;
    }
    
    // 尝试配置为输入带下拉
    ret = gpio_pin_configure(gpio0_dev, P0_05_PIN, GPIO_INPUT | GPIO_PULL_DOWN);
    if (ret < 0) {
        LOG_ERR("Failed to configure P0.05 with pull-down: %d", ret);
        return ret;
    }
    
    LOG_INF("P0.05 configured as input with pull-down");
    return 0;
}

// 备用方案：配置为强输出低电平
static int configure_p0_05_strong_output(void)
{
    int ret;
    
    if (!gpio0_dev) {
        gpio0_dev = DEVICE_DT_GET(P0_05_PORT);
    }
    
    if (!device_is_ready(gpio0_dev)) {
        LOG_ERR("GPIO0 device not ready");
        return -ENODEV;
    }
    
    // 配置为强输出，初始低电平
    ret = gpio_pin_configure(gpio0_dev, P0_05_PIN, GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOW);
    if (ret < 0) {
        LOG_ERR("Failed to configure P0.05 as strong output: %d", ret);
        return ret;
    }
    
    LOG_WRN("P0.05 configured as STRONG OUTPUT (low) - alternative approach");
    return 0;
}

// 检查P0.05当前状态
static void check_p0_05_state(void)
{
    if (!gpio0_dev || !device_is_ready(gpio0_dev)) {
        return;
    }
    
    int state = gpio_pin_get(gpio0_dev, P0_05_PIN);
    static int last_state = -1;
    
    if (state != last_state) {
        LOG_INF("P0.05 state changed: %d -> %d", last_state, state);
        last_state = state;
    }
}

// 定期修复处理函数
static void fix_handler(struct k_work *work)
{
    static int fix_count = 0;
    
    // 定期重新配置P0.05
    int ret = configure_p0_05_strong_pull();
    
    if (ret != 0) {
        LOG_WRN("Pull-down configuration failed, trying output method");
        configure_p0_05_strong_output();
    }
    
    fix_count++;
    
    // 检查状态（仅在前几次和偶尔检查）
    if (fix_count <= 5 || (fix_count % 20 == 0)) {
        check_p0_05_state();
    }
    
    // 动态调整修复频率
    uint32_t delay_ms = (fix_count < 10) ? 100 :  // 前期快速修复
                        (fix_count < 30) ? 500 :  // 中期中等频率
                        1000;                     // 后期较低频率
    
    k_work_reschedule(&fix_work, K_MSEC(delay_ms));
}

static int gpio_p0_05_fix_init(void)
{
    LOG_INF("Initializing P0.05 voltage fix");
    
    // 获取GPIO设备
    gpio0_dev = DEVICE_DT_GET(P0_05_PORT);
    if (!device_is_ready(gpio0_dev)) {
        LOG_ERR("GPIO0 device not ready at init");
        return -ENODEV;
    }
    
    // 立即应用修复
    int ret = configure_p0_05_strong_pull();
    if (ret != 0) {
        LOG_WRN("Initial pull-down failed, trying output method");
        configure_p0_05_strong_output();
    }
    
    // 记录初始状态
    check_p0_05_state();
    
    // 启动持续修复
    k_work_init_delayable(&fix_work, fix_handler);
    k_work_reschedule(&fix_work, K_MSEC(50));
    
    LOG_INF("P0.05 voltage fix initialized");
    return 0;
}

SYS_INIT(gpio_p0_05_fix_init, POST_KERNEL, 70);