#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

void init_clock();
void init_timer();
unsigned int get_clock();
void timer_resume();
void timer_pause();
unsigned int get_time();
void wait();


#endif // TIMER_H_INCLUDED
