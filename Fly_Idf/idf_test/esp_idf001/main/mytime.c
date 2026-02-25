#include "mytime.h"
static gptimer_handle_t gptimer = NULL;
static gptimer_config_t timer_config = {0};
static uint64_t last_ms = 0;
static uint64_t count;

void time_init(void)
{

    timer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT,   // 选择默认的时钟源
    timer_config.direction = GPTIMER_COUNT_UP,    // 计数方向为向上计数
    timer_config.resolution_hz = 1 * 1000 * 1000, // 分辨率为 1 MHz，即 1 次滴答为 1 微秒
    // 创建定时器实例
    (gptimer_new_timer(&timer_config, &gptimer));
    // 使能定时器
    (gptimer_enable(gptimer));
    // 启动定时器
    (gptimer_start(gptimer));
}

// 获取当前微秒数（1?s 精度）
uint64_t time_get_us(void)
{
    // 查看定时器的分辨率
    uint32_t resolution_hz;
    (gptimer_get_resolution(gptimer, &resolution_hz));
    (gptimer_get_raw_count(gptimer, &count));
    // （可选的）将计数值转换成时间单位 (秒)
    double time = (double)count / resolution_hz;
    return (uint64_t)(time * 1000000); // 转换为微秒
}

uint64_t time_get_ms(void)
{
    return time_get_us() / 1000;
}

// 核心：获取 dt（秒），PID 直接用
float time_get_dt(void)
{
    uint64_t now_ms = time_get_ms();
    float dt = (now_ms - last_ms) / 1000.0f;
    last_ms = now_ms;
    return dt;
}

// 阻塞延时 us
void mydelay_us(uint64_t us)
{
    uint64_t start = time_get_us();
    while ((time_get_us() - start) < us);
}

// 阻塞延时 ms
void mydelay_ms(uint32_t ms)
{
    mydelay_us((uint64_t)ms * 1000);
}
