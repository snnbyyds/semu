#ifndef __TIMER_DEVICE_H__
#define __TIMER_DEVICE_H__

#define TIMER_HZ 60

void add_timer_exec(void (*subhandler)(void));
void stop_timers();
void resume_timers();

#endif
