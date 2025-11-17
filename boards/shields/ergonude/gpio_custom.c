#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define P0_05_PORT DT_NODELABEL(gpio0)
#define P0_05_PIN  5

// 使用系统提供的nRF头文件
#include <hal/nrf_gpio.h>

static const struct device *gpio0_dev;

// 直接使用nRF GPIO HAL配置P0.05
static void direct_configure_p0_05(void)
{
    LOG_INF("Configuring P0.05 via nRF GPIO HAL");
    
    // 使用nRF HAL直接配置引脚
    nrf_gpio_cfg_input(P0_05_PIN, NRF_GPIO_PIN_PULLDOWN);
    
    LOG_INF("P0.05 configured using nRF GPIO HAL");
}

// 验证配置
static void verify_p0_05_configuration(void)
{
    // 读取PIN_CNF寄存器
    uint32_t pincnf = nrf_gpio_pin_cfg_get(P0_05_PIN);
    
    LOG_INF("P0.05 PIN_CNF[%d] = 0x%08x", P0_05_PIN, pincnf);
    
    // 解析配置
    uint32_t dir = (pincnf & GPIO_PIN_CNF_DIR_Msk) >> GPIO_PIN_CNF_DIR_Pos;
    uint32_t input = (pincnf & GPIO_PIN_CNF_INPUT_Msk) >> GPIO_PIN_CNF_INPUT_Pos;
    uint32_t pull = (pincnf & GPIO_PIN_CNF_PULL_Msk) >> GPIO_PIN_CNF_PULL_Pos;
    
    LOG_INF("P0.05 config - DIR: %s, INPUT: %s, PULL: %s",
           dir == GPIO_PIN_CNF_DIR_Input ? "Input" : "Output",
           input == GPIO_PIN_CNF_INPUT_Connect ? "Connected" : "Disconnected",
           pull == GPIO_PIN_CNF_PULL_DOWN ? "PullDown" : 
           pull == GPIO_PIN_CNF_PULL_UP ? "PullUp" : "Disabled");
}

// 使用Zephyr API作为备用
static int zephyr_configure_p0_05(void)
{
    if (!gpio0_dev) {
        gpio0_dev = DEVICE_DT_GET(P0_05_PORT);
    }
    
    if (!device_is_ready(gpio0_dev)) {
        LOG_ERR("GPIO0 device not ready");
        return -ENODEV;
    }
    
    int ret = gpio_pin_configure(gpio0_dev, P0_05_PIN, GPIO_INPUT | GPIO_PULL_DOWN);
    if (ret == 0) {
        LOG_INF("P0.05 Zephyr configuration successful");
    } else {
        LOG_ERR("P0.05 Zephyr configuration failed: %d", ret);
    }
    
    return ret;
}

// 读取引脚状态
static void read_p0_05_state(void)
{
    // 通过Zephyr API读取
    if (gpio0_dev && device_is_ready(gpio0_dev)) {
        int state = gpio_pin_get(gpio0_dev, P0_05_PIN);
        LOG_INF("P0.05 state (Zephyr API): %d", state);
    }
}

static int gpio_p0_05_hal_init(void)
{
    LOG_INF("=== P0.05 HAL INITIALIZATION ===");
    
    // 延迟确保系统基本初始化完成
    k_msleep(100);
    
    // 方法1: 使用nRF HAL直接配置
    direct_configure_p0_05();
    
    // 验证配置
    verify_p0_05_configuration();
    
    // 方法2: Zephyr API配置（备用）
    zephyr_configure_p0_05();
    
    // 读取初始状态
    read_p0_05_state();
    
    LOG_INF("=== P0.05 HAL INITIALIZATION COMPLETE ===");
    
    return 0;
}

SYS_INIT(gpio_p0_05_hal_init, POST_KERNEL, 20);