#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <e_ctimers.h>

/// see also e_mesh_transmitter.c in epiphany-examples/emesh/emesh_traffic/src
#define E_CTIMER_MESH_0 (0xf)
#define E_CTIMER_MESH_1 (0xe)

#define E_MESHEVENT_ALL1        0x00000200
#define E_MESHEVENT_COREWAIT1   0x00000300
/// todo: other mesh events


#define PREPAREMESHTIMER1(mesh_reg, event_type) mesh_reg = e_reg_read(E_REG_MESHCFG);       mesh_reg_timer = mesh_reg & 0xfffff00f;	mesh_reg_timer = mesh_reg_timer | event_type;	e_reg_write(E_REG_MESHCFG, mesh_reg_timer);
#define STARTMESHTIMER1(start_ticks)            e_ctimer_set(E_CTIMER_1, E_CTIMER_MAX) ;    start_ticks = e_ctimer_start(E_CTIMER_1, E_CTIMER_MESH_1);
#define STOPMESHTIMER1(stop_ticks)              stop_ticks = e_ctimer_get(E_CTIMER_1);      e_ctimer_stop(E_CTIMER_1);
#define RESETMESHTIMER1(mesh_reg)               e_reg_write(E_REG_MESHCFG, mesh_reg);


#define STARTCLOCK0(start_ticks)         e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);   e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);    start_ticks = e_ctimer_get(E_CTIMER_0);
#define STOPCLOCK0(stop_ticks)           stop_ticks = e_ctimer_get(E_CTIMER_0);    e_ctimer_stop(E_CTIMER_0);

#define STARTIDLECOUNTER1(start_ticks)   e_ctimer_set(E_CTIMER_1, E_CTIMER_MAX);   e_ctimer_start(E_CTIMER_1, E_CTIMER_IDLE);   start_ticks = e_ctimer_get(E_CTIMER_1);
#define STOPIDLECOUNTER1(stop_ticks)     stop_ticks = e_ctimer_get(E_CTIMER_1);    e_ctimer_stop(E_CTIMER_1);


#endif // TIMER_H_INCLUDED
