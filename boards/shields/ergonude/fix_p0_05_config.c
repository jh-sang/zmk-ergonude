/* fix_p0_05_config.c */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

static int fix_p0_05_config(void) {
    const struct device *gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
    
    if (!device_is_ready(gpio0)) {
        return -ENODEV;
    }
    
    // 尝试使用下拉电阻而不是上拉
    int ret = gpio_pin_configure(gpio0, 5, GPIO_INPUT | GPIO_PULL_DOWN);
    
    if (ret == 0) {
        printk("P0.05 configured with PULL-DOWN\n");
        
        // 测试读取
        int state = gpio_pin_get(gpio0, 5);
        printk("P0.05 state after pull-down config: %d\n", state);
    } else {
        printk("Failed to configure P0.05 with pull-down: %d\n", ret);
    }
    
    return ret;
}

SYS_INIT(fix_p0_05_config, APPLICATION, 90);