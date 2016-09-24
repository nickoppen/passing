#include "ringTopo16.c"
#include "passing.h"
#include "timer.h"
//#include "timer.c"
#include <coprthr.h>
//#include <coprthr2.h>
#include <coprthr_mpi.h>

#include "esyscall.h"

#include <host_stdio.h>

//#define REPEATCOUNT 100000
#define REPEATCOUNT 1000
#define MPI_BUF_SIZE 1024

/// global for debugging
//int _debugSpace[256];

void initLocal(int * vLocal, int n, unsigned int base)
{
    unsigned int firstI, lastI, i;

    firstI = base * n;                       /// the data to be passed is based on the core's global_id. This favours broadcast which is just iterates through the global ids writing to all. Unicast and Multicast are more complex because they
    lastI = firstI + n;                     /// write to the cores neighbour(s) which may not have a gid + or - 1. Therefore the gidOrder array must be looked up to see where the data came from.
    for (i=firstI; i<lastI; i++)
        vLocal[i] = base + 100;            /// ADD 100 to distinguish the initialised value from the default value which is 0

}


///
/// Broadcast: transmit the node values calculated here to all other cores without waiting between sends.
///
void __entry k_passBroadcast(pass_args * pArgs)
{
    unsigned int gid = coprthr_get_thread_id();
    unsigned int localCoreId;
    unsigned int i, j, coreI;
    unsigned int d = 0;
    unsigned int firstI, lastI;
    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a litte
    int n = pArgs->n; /// local copy of g_n
    unsigned time_e, time_s, idle_e, idle_s;

//    int vLocal[16384];  // 1024 * 16      // int must be two bytes or more
    int vLocal[1024];
    unsigned int coreS1andS2[] = {core00, core01, core02, core03, core10, core11, core12, core13, core20, core21, core22, core23, core30, core31, core32, core33};
    unsigned int coreS3[] = {core30, core20, core22, core00, core23, core11, core32, core31, core13, core10, core33, core03, core21, core12, core01, core02};       ///www.random.org/sequences

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
//    host_printf("k_passBroadcastNoWait: thread %i has debug[gid] at %i\n", gid, pArgs->debug[gid]);

    initLocal(vLocal, n, gid);

///     Uncomment these lines to see the vLocal array before any data is passed - make sure that you uncomment the barrier otherwise the other cores will start writing into the lcoal array before you have copied the data
//    if (gid == 6)
//        for (j=0; j< (CORECOUNT*n); j++)
//            g_debug[d++] = vLocal[j];
//            coprthr_barrier(0);

    firstI = gid * n;
    lastI = firstI + n;
    localCoreId = LOCAL_MEM_ADDRESS_BASE();

    coprthr_barrier(0);     /// ensure that all cores start at the same time
    STARTCLOCK(time_s);
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
    STOPCLOCK(time_e);
    coprthr_barrier(0); /// ensure that all cores are done before recording results and doing the host printf
    pArgs->debug[gid] = time_s - time_e;
//    host_printf("Pass_Broardcast(pass up): Core with rank: %i, total time :%i\n", gid, time_s - time_e);

/// testing - 1
    repeater = REPEATCOUNT;

    coprthr_barrier(0);
    STARTCLOCK(time_s);
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
    STOPCLOCK(time_e);
    coprthr_barrier(0);
    pArgs->debug[gid+16] = time_s - time_e;
//    host_printf("Pass_Broardcast(pass 0 to 15): Core with rank: %i, total time :%i\n", gid, time_s - time_e);

/// testing - random
    repeater = REPEATCOUNT;

    coprthr_barrier(0);
    STARTCLOCK(time_s);
    core = coreS3;
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
    STOPCLOCK(time_e);
    coprthr_barrier(0);
    pArgs->debug[gid+32] = time_s - time_e;
//    host_printf("Pass_Broardcast(random): Core with rank: %i, total time :%i\n", gid, time_s - time_e);

/// testing - 2 4 way map
    repeater = REPEATCOUNT;

    coprthr_barrier(0);
    STARTCLOCK(time_s);
    int * coreS4;
    switch (gid)
    {
        case 0:
            core = coreGid00;
        break;
        case 1:
            core - coreGid01;
        break;
        case 2:
            core - coreGid02;
        break;
        case 3:
            core - coreGid03;
        break;
        case 4:
            core - coreGid04;
        break;
        case 5:
            core - coreGid05;
        break;
        case 6:
            core - coreGid06;
        break;
        case 7:
            core - coreGid07;
        break;
        case 8:
            core - coreGid08;
        break;
        case 9:
            core - coreGid09;
        break;
        case 10:
            core - coreGid10;
        break;
        case 11:
            core - coreGid11;
        break;
        case 12:
            core - coreGid12;
        break;
        case 13:
            core - coreGid13;
        break;
        case 14:
            core - coreGid14;
        break;
        case 15:
            core - coreGid15;
        break;
    }
    while (repeater--)
    {
        for(coreI = 0; coreI < MAXCORE; coreI++)
        {
            if (core[coreI] != localCoreId)
                for (i=firstI; i < lastI; i++)
                    *(int *)NEIGHBOUR_LOC(core[coreI], vLocal,  i, (sizeof(int))) = vLocal[i];
        }
    }
    STOPCLOCK(time_e);
    coprthr_barrier(0);
    pArgs->debug[gid+48] = time_s - time_e;
//    host_printf("Pass_Broardcast(4 way map): Core with rank: %i, total time :%i\n", gid, time_s - time_e);

/// Uncomment to use g_debug to show the final array contents
//                    if (gid == 6)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            g_debug[d++] = vLocal[j];

}

