/// Thanks to Albert Netymk

#include <e_ctimers.h>
#include <stdbool.h>

bool initialized = false;

void init_clock()
{
    e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
    e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
}

unsigned int get_clock()
{
    return E_CTIMER_MAX - e_ctimer_get(E_CTIMER_0);
}

void init_timer()
{
    initialized = true;
    e_ctimer_set(E_CTIMER_1, E_CTIMER_MAX);
}

void timer_resume()
{
    if (!initialized) {
        init_timer();
        init_clock();
    }
    e_ctimer_start(E_CTIMER_1, E_CTIMER_CLK);
}

void timer_pause()
{
    e_ctimer_stop(E_CTIMER_1);
}

unsigned int get_time()
{
    return E_CTIMER_MAX - e_ctimer_get(E_CTIMER_1);
}

void wait()
{
    timer_resume();
}
