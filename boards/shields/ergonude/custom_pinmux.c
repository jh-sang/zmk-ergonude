#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>

static int custom_pinmux_init(void) {
    ARG_UNUSED(); // 如果没有参数需要处理
    
    const struct device *gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
    
    if (!device_is_ready(gpio0)) {
        printk("GPIO0 device not ready\n");
        return -ENODEV;
    }
    
    /* 重新配置 P0.05 为矩阵行输入引脚 */
    int ret = gpio_pin_configure(gpio0, 5, GPIO_INPUT);
    
    if (ret == 0) {
        printk("Successfully configured P0.05 as matrix row input\n");
        
        // 验证配置
        int val = gpio_pin_get(gpio0, 5);
        printk("P0.05 current value: %d\n", val);
    } else {
        printk("Failed to configure P0.05: %d\n", ret);
    }
    
    return ret;
}

SYS_INIT(custom_pinmux_init, POST_KERNEL, 80);