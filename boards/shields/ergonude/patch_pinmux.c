/* override_pinmux.c */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>

/* 声明原函数为弱符号，这样我们可以覆盖它 */
__weak int pinmux_nrfmicro_init(void);

/* 我们的实现覆盖弱符号 */
int pinmux_nrfmicro_init(void) {
    /* 只执行必要的初始化，跳过 p0.05 的特殊处理 */
#if (CONFIG_BOARD_NRFMICRO_13 || CONFIG_BOARD_NRFMICRO_13_52833)
    const struct device *p0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
    
    if (device_is_ready(p0)) {
        /* 对于 p0.05，我们只配置为输入，不进行充电器相关配置 */
        gpio_pin_configure(p0, 5, GPIO_INPUT);
        
        /* 其他原有的初始化可以保留 */
        // ... 其他引脚的初始化
    }
#endif
    return 0;
}