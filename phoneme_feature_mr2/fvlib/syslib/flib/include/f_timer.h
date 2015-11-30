#ifndef __F_TIMER_H__
#define __F_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*f_timer_func)(void* arg);

unsigned int f_timer_monotime_ms(void);
unsigned int f_timer_ms(void);
int f_timer_init();
unsigned int f_timer_add(int interval, f_timer_func func, void* arg);
int f_timer_remove(unsigned int timer_id);
int f_timer_fire();

#ifdef __cplusplus
}
#endif

#endif
