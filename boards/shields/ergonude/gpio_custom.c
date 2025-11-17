#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define P0_05_PORT DT_NODELABEL(gpio0)
#define P0_05_PIN  5

static const struct device *gpio0_dev;
static struct k_work_delayable fix_work;

// 配置P0.05为输入下拉
static int configure_p0_05(void)
{
    int ret;
    
    if (!gpio0_dev) {
        gpio0_dev = DEVICE_DT_GET(P0_05_PORT);
    }
    
    if (!device_is_ready(gpio0_dev)) {
        LOG_ERR("GPIO0 device not ready");
        return -ENODEV;
    }
    
    // 配置为输入带下拉
    ret = gpio_pin_configure(gpio0_dev, P0_05_PIN, GPIO_INPUT | GPIO_PULL_DOWN);
    if (ret < 0) {
        LOG_ERR("Failed to configure P0.05: %d", ret);
        return ret;
    }
    
    return 0;
}

// 读取并记录P0.05状态
static void monitor_p0_05(void)
{
    static int last_state = -1;
    
    if (!gpio0_dev || !device_is_ready(gpio0_dev)) {
        return;
    }
    
    int current_state = gpio_pin_get(gpio0_dev, P0_05_PIN);
    
    if (current_state != last_state) {
        LOG_INF("P0.05 state changed: %d -> %d", last_state, current_state);
        last_state = current_state;
    }
}

// 修复工作处理函数
static void fix_handler(struct k_work *work)
{
    static int fix_count = 0;
    
    // 定期重新配置
    int ret = configure_p0_05();
    
    if (ret == 0) {
        fix_count++;
        
        // 减少日志输出频率
        if (fix_count <= 5 || (fix_count % 20 == 0)) {
            LOG_INF("P0.05 fix applied (%d times)", fix_count);
            
            // 监控状态变化
            monitor_p0_05();
        }
    } else {
        LOG_ERR("P0.05 configuration failed on attempt %d", fix_count);
    }
    
    // 动态调整频率
    uint32_t delay_ms = (fix_count < 10) ? 100 :
                        (fix_count < 30) ? 500 :
                        2000;
    
    k_work_reschedule(&fix_work, K_MSEC(delay_ms));
}

static int gpio_p0_05_fix_init(void)
{
    LOG_INF("Starting P0.05 GPIO fix");
    
    // 立即配置一次
    configure_p0_05();
    
    // 启动持续修复
    k_work_init_delayable(&fix_work, fix_handler);
    k_work_reschedule(&fix_work, K_MSEC(50));
    
    return 0;
}

SYS_INIT(gpio_p0_05_fix_init, POST_KERNEL, 50);