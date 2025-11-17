#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define P0_05_PORT DT_NODELABEL(gpio0)
#define P0_05_PIN  5

// nRF52840 GPIO寄存器定义
#define NRF_GPIO_BASE 0x50000000
#define GPIO_PIN_CNF(n) (*(volatile uint32_t *)(NRF_GPIO_BASE + 0x700 + (n * 0x4)))

// PIN_CNF寄存器位定义
#define GPIO_PIN_CNF_DIR_Pos          0
#define GPIO_PIN_CNF_DIR_Msk          (0x1UL << GPIO_PIN_CNF_DIR_Pos)
#define GPIO_PIN_CNF_DIR_Input        (0x0UL << GPIO_PIN_CNF_DIR_Pos)
#define GPIO_PIN_CNF_DIR_Output       (0x1UL << GPIO_PIN_CNF_DIR_Pos)

#define GPIO_PIN_CNF_INPUT_Pos        1
#define GPIO_PIN_CNF_INPUT_Msk        (0x1UL << GPIO_PIN_CNF_INPUT_Pos)
#define GPIO_PIN_CNF_INPUT_Connect    (0x0UL << GPIO_PIN_CNF_INPUT_Pos)
#define GPIO_PIN_CNF_INPUT_Disconnect (0x1UL << GPIO_PIN_CNF_INPUT_Pos)

#define GPIO_PIN_CNF_PULL_Pos         2
#define GPIO_PIN_CNF_PULL_Msk         (0x3UL << GPIO_PIN_CNF_PULL_Pos)
#define GPIO_PIN_CNF_PULL_Disabled    (0x0UL << GPIO_PIN_CNF_PULL_Pos)
#define GPIO_PIN_CNF_PULL_Down        (0x1UL << GPIO_PIN_CNF_PULL_Pos)
#define GPIO_PIN_CNF_PULL_Up          (0x3UL << GPIO_PIN_CNF_PULL_Pos)

#define GPIO_PIN_CNF_DRIVE_Pos        8
#define GPIO_PIN_CNF_DRIVE_Msk        (0x7UL << GPIO_PIN_CNF_DRIVE_Pos)
#define GPIO_PIN_CNF_DRIVE_S0S1       (0x0UL << GPIO_PIN_CNF_DRIVE_Pos)

#define GPIO_PIN_CNF_SENSE_Pos        16
#define GPIO_PIN_CNF_SENSE_Msk        (0x3UL << GPIO_PIN_CNF_SENSE_Pos)
#define GPIO_PIN_CNF_SENSE_Disabled   (0x0UL << GPIO_PIN_CNF_SENSE_Pos)

static const struct device *gpio0_dev;

// 直接通过内存映射寄存器配置P0.05
static void direct_register_configure_p0_05(void)
{
    LOG_INF("Configuring P0.05 via direct register access");
    
    // 配置P0.05为输入，带下拉，标准驱动强度，无感应
    uint32_t pincnf_value = (GPIO_PIN_CNF_DIR_Input |
                           GPIO_PIN_CNF_INPUT_Connect |
                           GPIO_PIN_CNF_PULL_Down |
                           GPIO_PIN_CNF_DRIVE_S0S1 |
                           GPIO_PIN_CNF_SENSE_Disabled);
    
    GPIO_PIN_CNF(P0_05_PIN) = pincnf_value;
    
    LOG_INF("P0.05 PIN_CNF[%d] = 0x%08x", P0_05_PIN, pincnf_value);
}

// 验证寄存器配置
static void verify_register_config(void)
{
    uint32_t current_config = GPIO_PIN_CNF(P0_05_PIN);
    LOG_INF("P0.05 current PIN_CNF[%d] = 0x%08x", P0_05_PIN, current_config);
    
    // 检查关键配置位
    uint32_t dir = (current_config & GPIO_PIN_CNF_DIR_Msk) >> GPIO_PIN_CNF_DIR_Pos;
    uint32_t pull = (current_config & GPIO_PIN_CNF_PULL_Msk) >> GPIO_PIN_CNF_PULL_Pos;
    uint32_t input = (current_config & GPIO_PIN_CNF_INPUT_Msk) >> GPIO_PIN_CNF_INPUT_Pos;
    
    LOG_INF("P0.05 config - DIR: %s, INPUT: %s, PULL: %s",
           dir == 0 ? "Input" : "Output",
           input == 0 ? "Connected" : "Disconnected",
           pull == 0 ? "Disabled" : (pull == 1 ? "Down" : "Up"));
}

// 使用Zephyr API作为备用方案
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

// 读取引脚状态的多种方法
static void read_p0_05_state(void)
{
    // 方法1: 通过Zephyr API读取
    if (gpio0_dev && device_is_ready(gpio0_dev)) {
        int state = gpio_pin_get(gpio0_dev, P0_05_PIN);
        LOG_INF("P0.05 state (Zephyr API): %d", state);
    }
    
    // 方法2: 通过IN寄存器直接读取
    volatile uint32_t *gpio_in = (volatile uint32_t *)(NRF_GPIO_BASE + 0x510);
    uint32_t port_state = *gpio_in;
    int pin_state = (port_state >> P0_05_PIN) & 0x1;
    LOG_INF("P0.05 state (direct IN register): %d", pin_state);
}

static int gpio_p0_05_direct_init(void)
{
    LOG_INF("=== DIRECT REGISTER P0.05 INITIALIZATION ===");
    
    // 延迟确保系统基本初始化完成
    k_msleep(100);
    
    // 方法1: 直接寄存器配置（主要方法）
    direct_register_configure_p0_05();
    
    // 验证寄存器配置
    verify_register_config();
    
    // 方法2: Zephyr API配置（备用）
    zephyr_configure_p0_05();
    
    // 读取初始状态
    read_p0_05_state();
    
    LOG_INF("=== P0.05 DIRECT INITIALIZATION COMPLETE ===");
    
    return 0;
}

SYS_INIT(gpio_p0_05_direct_init, POST_KERNEL, 20);