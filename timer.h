#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <e_ctimers.h>

//void init_clock();
//void init_timer();
//unsigned int get_clock();
//void timer_resume();
//void timer_pause();
//unsigned int get_time();
//void wait();

#define STARTCLOCK(start_ticks)     e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);    e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);    start_ticks = e_ctimer_get(E_CTIMER_0);
#define STOPCLOCK(stop_ticks)     stop_ticks = e_ctimer_get(E_CTIMER_0);    e_ctimer_stop(E_CTIMER_0);

#define STARTIDLECOUNTER(start_ticks)     e_ctimer_set(E_CTIMER_1, E_CTIMER_MAX);    e_ctimer_start(E_CTIMER_1, E_CTIMER_IDLE);    start_ticks = e_ctimer_get(E_CTIMER_1);
#define STOPIDLECOUNTER(stop_ticks)     stop_ticks = e_ctimer_get(E_CTIMER_1);    e_ctimer_stop(E_CTIMER_1);


#endif // TIMER_H_INCLUDED
