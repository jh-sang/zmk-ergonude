#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>

static int custom_pinmux_init(void) {
    const struct device *gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
    
    if (!device_is_ready(gpio0)) {
        printk("GPIO0 device not ready\n");
        return -ENODEV;
    }
    
    // 更长的延迟确保原初始化完全完成
    k_msleep(50);
    
    /* 配置为输入模式，根据你的矩阵电路选择上拉或下拉 */
    int ret = gpio_pin_configure(gpio0, 5, GPIO_INPUT | GPIO_PULL_DOWN);
    
    if (ret == 0) {
        printk("Custom pinmux: P0.05 configured as matrix row\n");
    } else {
        printk("Custom pinmux: Failed to configure P0.05: %d\n", ret);
    }
    
    return ret;
}

// 使用更高的优先级确保在原有初始化之后运行
SYS_INIT(custom_pinmux_init, POST_KERNEL, 95);