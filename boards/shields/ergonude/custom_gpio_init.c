// 在自定义代码文件中添加
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>

static int custom_gpio_init(const struct device *dev)
{
    ARG_UNUSED(dev);
    
    const struct device *gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
    
    if (!device_is_ready(gpio0)) {
        return -ENODEV;
    }
    
    // 强制配置P0.05为输入下拉
    gpio_pin_configure(gpio0, 5, GPIO_INPUT | GPIO_PULL_DOWN);
    
    return 0;
}

// 在系统启动时早期初始化
SYS_INIT(custom_gpio_init, POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY);