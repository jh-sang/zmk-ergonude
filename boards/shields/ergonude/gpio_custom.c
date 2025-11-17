#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define P0_05_PORT DT_NODELABEL(gpio0)
#define P0_05_PIN  5

static const struct device *gpio0_dev;
static struct k_work_delayable matrix_state_work;
static int last_other_row_active = 0;

// 监控其他行是否活跃
static int check_other_rows_active(void)
{
    // 这里我们需要监控其他行的状态
    // 由于我们无法直接访问ZMK的矩阵状态，我们通过监控P0.05的状态变化来推断
    
    if (!gpio0_dev || !device_is_ready(gpio0_dev)) {
        return 0;
    }
    
    // 读取P0.05状态
    int p0_05_state = gpio_pin_get(gpio0_dev, P0_05_PIN);
    
    // 如果P0.05状态为高，可能表示其他行正在被扫描
    // 这是一个间接的推断方法
    return p0_05_state;
}

// 强力修复P0.05配置
static void force_fix_p0_05(void)
{
    static int fix_count = 0;
    
    if (!gpio0_dev) {
        gpio0_dev = DEVICE_DT_GET(P0_05_PORT);
    }
    
    if (!device_is_ready(gpio0_dev)) {
        return;
    }
    
    // 强力重置：先输出低，再输入下拉
    gpio_pin_configure(gpio0_dev, P0_05_PIN, GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOW);
    k_busy_wait(50);
    gpio_pin_configure(gpio0_dev, P0_05_PIN, GPIO_INPUT | GPIO_PULL_DOWN);
    
    fix_count++;
    
    if (fix_count <= 5 || (fix_count % 50 == 0)) {
        LOG_INF("P0.05 force fix applied (%d times)", fix_count);
    }
}

// 矩阵状态感知修复
static void matrix_state_aware_fix(void)
{
    int current_other_active = check_other_rows_active();
    
    // 检测其他行从非活跃变为活跃的状态变化
    if (current_other_active && !last_other_row_active) {
        LOG_INF("Other rows became active - applying P0.05 fix");
        force_fix_p0_05();
    }
    
    last_other_row_active = current_other_active;
}

static void matrix_state_handler(struct k_work *work)
{
    // 应用矩阵状态感知修复
    matrix_state_aware_fix();
    
    // 持续监控矩阵状态
    k_work_reschedule(&matrix_state_work, K_MSEC(10)); // 100Hz监控
}

static int gpio_p0_05_matrix_aware_init(void)
{
    LOG_INF("=== STARTING MATRIX-AWARE P0.05 FIX ===");
    
    gpio0_dev = DEVICE_DT_GET(P0_05_PORT);
    
    if (!device_is_ready(gpio0_dev)) {
        LOG_ERR("GPIO0 not ready at init");
    } else {
        // 初始配置
        force_fix_p0_05();
    }
    
    // 启动矩阵状态监控
    k_work_init_delayable(&matrix_state_work, matrix_state_handler);
    k_work_reschedule(&matrix_state_work, K_MSEC(20));
    
    return 0;
}

SYS_INIT(gpio_p0_05_matrix_aware_init, POST_KERNEL, 40);