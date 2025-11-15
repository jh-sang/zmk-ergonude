/* 自定义引脚配置文件 - custom_pinmux.c */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>

static int custom_pinmux_init(const struct device *arg) {
    ARG_UNUSED(arg);
    
    const struct device *gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
    
    if (!device_is_ready(gpio0)) {
        return -ENODEV;
    }
    
    /* 将 p0.05 重新配置为矩阵行引脚（输入模式） */
    int ret = gpio_pin_configure(gpio0, 5, GPIO_INPUT | GPIO_PULL_UP);
    if (ret < 0) {
        printk("Failed to configure P0.05 as matrix row (err %d)\n", ret);
        return ret;
    }
    
    printk("Custom pinmux: P0.05 configured as matrix row\n");
    return 0;
}

/* 使用 POST_KERNEL 优先级，确保在原初始化之后运行 */
SYS_INIT(custom_pinmux_init, POST_KERNEL, 90);