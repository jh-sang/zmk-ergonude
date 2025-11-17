#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define P0_05_PORT DT_NODELABEL(gpio0)
#define P0_05_PIN  5

static const struct device *gpio0_dev;
static struct k_work_delayable simulate_work;

// 模拟其他行活动的效果
static void simulate_other_row_activity(void)
{
    static int simulate_count = 0;
    
    if (!gpio0_dev) {
        gpio0_dev = DEVICE_DT_GET(P0_05_PORT);
    }
    
    if (!device_is_ready(gpio0_dev)) {
        return;
    }
    
    // 方法：定期短暂地将P0.05配置为输出低电平，模拟其他行扫描的效果
    // 这会"重置"P0.05的状态，使其能够响应按键
    
    // 保存当前配置
    int current_state = gpio_pin_get(gpio0_dev, P0_05_PIN);
    
    // 短暂配置为输出低电平（模拟其他行扫描）
    gpio_pin_configure(gpio0_dev, P0_05_PIN, GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOW);
    k_busy_wait(10); // 10us输出低电平
    
    // 立即恢复为输入下拉
    gpio_pin_configure(gpio0_dev, P0_05_PIN, GPIO_INPUT | GPIO_PULL_DOWN);
    
    simulate_count++;
    
    if (simulate_count <= 10 || (simulate_count % 100 == 0)) {
        LOG_INF("Simulated other row activity (%d times)", simulate_count);
        
        // 检查状态变化
        int new_state = gpio_pin_get(gpio0_dev, P0_05_PIN);
        if (new_state != current_state) {
            LOG_INF("P0.05 state changed after simulation: %d -> %d", current_state, new_state);
        }
    }
}

static void simulate_handler(struct k_work *work)
{
    simulate_other_row_activity();
    
    // 模拟频率：每100ms模拟一次其他行活动
    k_work_reschedule(&simulate_work, K_MSEC(100));
}

static int gpio_p0_05_simulate_init(void)
{
    LOG_INF("=== STARTING P0.05 ACTIVITY SIMULATION ===");
    
    gpio0_dev = DEVICE_DT_GET(P0_05_PORT);
    
    if (device_is_ready(gpio0_dev)) {
        // 初始配置
        gpio_pin_configure(gpio0_dev, P0_05_PIN, GPIO_INPUT | GPIO_PULL_DOWN);
    }
    
    // 启动活动模拟
    k_work_init_delayable(&simulate_work, simulate_handler);
    k_work_reschedule(&simulate_work, K_MSEC(50));
    
    return 0;
}

SYS_INIT(gpio_p0_05_simulate_init, POST_KERNEL, 35);