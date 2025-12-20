#ifndef USER_TIMER_H
#define USER_TIMER_H
#include <driver/timer.h>

void user_timer_init(int timer_num,
     int timer_divider, int timer_counter_dir, 
     int timer_counter_en, int timer_alarm_en, 
     int timer_auto_reload, void (*timer_isr_handler)(void *), void *arg);
// timer = timerBegin(0, 80, true); // 80·ÖÆµ£¬1us¼ÆÊý



#endif // USER_TIMER_H