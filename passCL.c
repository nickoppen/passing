#include "ringTopo16.c"
#include "passing.h"
#include "timer.h"
#include <coprthr.h>
#include <coprthr_mpi.h>

#include "esyscall.h"

#include <host_stdio.h>

#define REPEATCOUNT 1000
#define MPI_BUF_SIZE 1024

void initLocal(int * vLocal, int n, unsigned int base)
{
    unsigned int firstI, lastI, i;

    firstI = base * n;                       /// the data to be passed is based on the core's global_id. This favours broadcast which is just iterates through the global ids writing to all. Unicast and Multicast are more complex because they
    lastI = firstI + n;                     /// write to the cores neighbour(s) which may not have a gid + or - 1. Therefore the gidOrder array must be looked up to see where the data came from.
    for (i=firstI; i<lastI; i++)
        vLocal[i] = base + 100;            /// ADD 100 to distinguish the initialised value from the default value which is 0

}


///
/// Broadcast: send pArgs->n integers from the local core to all other cores
///
void __entry k_passBroadcast(pass_args * pArgs)
{
    unsigned int gid = coprthr_get_thread_id();
    unsigned int localCoreId;
    unsigned int i, j, coreI;
    unsigned int d = 0;
    unsigned int firstI, lastI;
    int n = pArgs->n; /// local copy of g_n
    unsigned int time_e, time_s, idle_e, idle_s;
    unsigned int mesh_reg,  mesh_reg_timer;

//    int vLocal[16384];  // 1024 * 16      // int must be two bytes or more
    int vLocal[1024];
    unsigned int coreS1andS2[] = {core00, core01, core02, core03, core10, core11, core12, core13, core20, core21, core22, core23, core30, core31, core32, core33};

    /// the sending core has been ommitted from each list
    unsigned int randCore00[] = {core02, core20, core23, core22, core21, core32, core31, core03, core01, core33, core10, core30, core13, core11, core12};       ///www.random.org/sequences
    unsigned int randCore01[] = {core33, core32, core30, core03, core20, core23, core21, core12, core31, core00, core11, core02, core01, core13, core22};
    unsigned int randCore02[] = {core02, core00, core22, core31, core32, core12, core13, core11, core21, core33, core01, core23, core03, core10, core30};
    unsigned int randCore03[] = {core23, core12, core21, core00, core03, core31, core33, core20, core11, core02, core01, core13, core10, core22, core32};
    unsigned int randCore04[] = {core02, core33, core12, core20, core31, core10, core22, core13, core11, core03, core00, core30, core32, core21, core23};
    unsigned int randCore05[] = {core30, core31, core23, core32, core22, core33, core20, core02, core13, core12, core21, core00, core03, core10, core01};
    unsigned int randCore06[] = {core30, core02, core32, core10, core23, core20, core00, core01, core13, core22, core31, core12, core03, core33, core11};
    unsigned int randCore07[] = {core11, core22, core21, core20, core23, core03, core10, core01, core00, core02, core33, core12, core32, core30, core13};
    unsigned int randCore08[] = {core20, core00, core33, core01, core22, core03, core13, core32, core12, core10, core21, core23, core11, core30, core31};
    unsigned int randCore09[] = {core01, core03, core33, core20, core00, core21, core11, core23, core13, core10, core32, core31, core30, core22, core02};
    unsigned int randCore10[] = {core31, core30, core11, core32, core33, core03, core23, core00, core01, core12, core21, core13, core02, core10, core20};
    unsigned int randCore11[] = {core30, core00, core31, core13, core33, core11, core23, core21, core01, core20, core12, core02, core22, core03, core10};
    unsigned int randCore12[] = {core32, core13, core12, core23, core31, core01, core21, core00, core11, core20, core02, core30, core33, core22, core10};
    unsigned int randCore13[] = {core22, core30, core10, core01, core03, core11, core21, core12, core33, core23, core02, core32, core31, core20, core00};
    unsigned int randCore14[] = {core32, core20, core21, core31, core22, core10, core13, core30, core00, core11, core33, core02, core12, core03, core01};
    unsigned int randCore15[] = {core13, core03, core12, core21, core22, core00, core30, core31, core02, core32, core11, core23, core10, core01, core20};


    unsigned int coreGid00[] = {core10, core01, core11, core20, core02, core21, core12, core30, core03, core22, core11, core13, core32, core23, core33};
    unsigned int coreGid01[] = {core20, core11, core00, core30, core12, core21, core01, core13, core31, core02, core22, core32, core03, core23, core33};
    unsigned int coreGid02[] = {core30, core21, core10, core22, core00, core31, core11, core23, core32, core12, core01, core33, core13, core02, core03};
    unsigned int coreGid03[] = {core31, core20, core32, core10, core21, core33, core00, core22, core11, core23, core12, core01, core13, core02, core03};
    unsigned int coreGid04[] = {core00, core11, core02, core21, core03, core10, core12, core31, core20, core22, core13, core30, core32, core23, core33};
    unsigned int coreGid05[] = {core10, core21, core12, core01, core31, core13, core20, core22, core02, core00, core30, core12, core23, core03, core33};
    unsigned int coreGid06[] = {core20, core31, core22, core11, core23, core01, core30, core32, core12, core10, core33, core13, core02, core00, core03};
    unsigned int coreGid07[] = {core30, core32, core21, core33, core11, core22, core20, core01, core23, core12, core10, core13, core02, core00, core03};
    unsigned int coreGid08[] = {core01, core12, core03, core00, core22, core11, core13, core32, core10, core21, core23, core20, core31, core33, core30};
    unsigned int coreGid09[] = {core02, core11, core22, core13, core10, core32, core01, core21, core23, core03, core00, core20, core31, core33, core30};
    unsigned int coreGid10[] = {core12, core21, core32, core23, core02, core20, core11, core31, core33, core13, core01, core10, core30, core03, core00};
    unsigned int coreGid11[] = {core22, core31, core33, core12, core30, core21, core23, core02, core11, core20, core13, core01, core03, core10, core00};
    unsigned int coreGid12[] = {core02, core13, core01, core23, core12, core00, core33, core11, core22, core10, core21, core32, core20, core31, core30};
    unsigned int coreGid13[] = {core03, core12, core23, core11, core33, core02, core22, core10, core01, core21, core32, core00, core20, core31, core30};
    unsigned int coreGid14[] = {core13, core22, core33, core03, core21, core12, core32, core20, core02, core11, core31, core12, core10, core30, core00};
    unsigned int coreGid15[] = {core23, core32, core13, core31, core22, core03, core30, core12, core21, core02, core11, core20, core01, core10, core00};
    unsigned int * core;

    initLocal(vLocal, n, gid);

    PREPAREMESHTIMER1(mesh_reg, E_MESHEVENT_ALL1);

/*
 *  The first broardcast strategy I wrote.
 *  Each core passes the data to gid+1, gid+2... core, wrapping round when gid == 15
 *  The core skips an iteration when the destination core is the same as the local core (i.e. it does not send data to itself)
 */
    firstI = gid * n;
    lastI = firstI + n;
    localCoreId = LOCAL_MEM_ADDRESS_BASE();

    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a litte
    coprthr_barrier(0);     /// ensure that all cores start at the same time
		e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
		idle_s = e_ctimer_start(E_CTIMER_1, E_CTIMER_MESH_1);

    STARTMESHTIMER1(idle_s);
    STARTCLOCK0(time_s);
    core = coreS1andS2;
    while (repeater--)
    {
        coreI = (gid == (CORECOUNT - 1)) ? 0 : gid + 1;     /// use the gid as a place to start in the core array
        while (coreI != gid)
        {
            if (core[coreI] != localCoreId)
                for (i=firstI; i < lastI; i++)
                    *(int *)NEIGHBOUR_LOC(core[coreI], vLocal,  i, (sizeof(int))) = vLocal[i];
            coreI = (coreI == (CORECOUNT - 1)) ? 0 : coreI + 1;
        }
    }
    STOPCLOCK0(time_e);
    STOPMESHTIMER1(idle_e);

    coprthr_barrier(0); /// ensure that all cores are done before recording results and doing the host printf
    pArgs->debug[gid] = time_s - time_e;
    pArgs->debug[gid+64] = idle_s - idle_e; /// put the idle time data after the clock data

/*
 *  The simplest strategy I could think of.
 *  Each core send the data to core00, core01... core33 in that order
 *  Again, the core does not send data to itself
 */
    repeater = REPEATCOUNT;

    coprthr_barrier(0);
    STARTMESHTIMER1(idle_s);
    STARTCLOCK0(time_s);
    core = coreS1andS2;
    while (repeater--)
    {
        for(coreI = 0; coreI < CORECOUNT; coreI++)
        {
            if (core[coreI] != localCoreId)
                for (i=firstI; i < lastI; i++)
                    *(int *)NEIGHBOUR_LOC(core[coreI], vLocal,  i, (sizeof(int))) = vLocal[i];
        }
    }
    STOPCLOCK0(time_e);
    STOPMESHTIMER1(idle_e);
    coprthr_barrier(0);
    pArgs->debug[gid+16] = time_s - time_e;
    pArgs->debug[gid+80] = idle_s - idle_e; /// put the idle time data after the clock data

/*
 *  Send data to each other core in random sequence. Each core has a different sequence.
 *  The sequence doesn not include the local core so it does not have to check and skip an iteration
 */
    repeater = REPEATCOUNT;

    coprthr_barrier(0);
    STARTMESHTIMER1(idle_s);
    STARTCLOCK0(time_s);
    switch (gid)
    {
        case 0:
            core = randCore00;
        break;
        case 1:
            core = randCore01;
        break;
        case 2:
            core = randCore02;
        break;
        case 3:
            core = randCore03;
        break;
        case 4:
            core = randCore04;
        break;
        case 5:
            core = randCore05;
        break;
        case 6:
            core = randCore06;
        break;
        case 7:
            core = randCore07;
        break;
        case 8:
            core = randCore08;
        break;
        case 9:
            core = randCore09;
        break;
        case 10:
            core = randCore10;
        break;
        case 11:
            core = randCore11;
        break;
        case 12:
            core = randCore12;
        break;
        case 13:
            core = randCore13;
        break;
        case 14:
            core = randCore14;
        break;
        case 15:
            core = randCore15;
        break;
    }

    while (repeater--)
    {
        for(coreI = 0; coreI < MAXCORE; coreI++)
        {
            for (i=firstI; i < lastI; i++)
                *(int *)NEIGHBOUR_LOC(core[coreI], vLocal,  i, (sizeof(int))) = vLocal[i];
        }
    }
    STOPCLOCK0(time_e);
    STOPMESHTIMER1(idle_e);
    coprthr_barrier(0);
    pArgs->debug[gid+32] = time_s - time_e;
    pArgs->debug[gid+96] = idle_s - idle_e; /// put the idle time data after the clock data

/*
 *  Pass the data to the other cores in a a planned, thought out manner
 *  Each core sequence does not include the local core
 */
    repeater = REPEATCOUNT;

    coprthr_barrier(0);
    STARTMESHTIMER1(idle_s);
    STARTCLOCK0(time_s);
    switch (gid)
    {
        case 0:
            core = coreGid00;
        break;
        case 1:
            core = coreGid01;
        break;
        case 2:
            core = coreGid02;
        break;
        case 3:
            core = coreGid03;
        break;
        case 4:
            core = coreGid04;
        break;
        case 5:
            core = coreGid05;
        break;
        case 6:
            core = coreGid06;
        break;
        case 7:
            core = coreGid07;
        break;
        case 8:
            core = coreGid08;
        break;
        case 9:
            core = coreGid09;
        break;
        case 10:
            core = coreGid10;
        break;
        case 11:
            core = coreGid11;
        break;
        case 12:
            core = coreGid12;
        break;
        case 13:
            core = coreGid13;
        break;
        case 14:
            core = coreGid14;
        break;
        case 15:
            core = coreGid15;
        break;
    }
    while (repeater--)
    {
        for(coreI = 0; coreI < MAXCORE; coreI++)
        {
            for (i=firstI; i < lastI; i++)
                *(int *)NEIGHBOUR_LOC(core[coreI], vLocal,  i, (sizeof(int))) = vLocal[i];
        }
    }
    STOPCLOCK0(time_e);
    STOPMESHTIMER1(idle_e);
    coprthr_barrier(0);
    pArgs->debug[gid+48] = time_s - time_e;
    pArgs->debug[gid+112] = idle_s - idle_e; /// put the idle time data after the clock data

	RESETMESHTIMER1(mesh_reg);


}

