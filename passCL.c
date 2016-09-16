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
#define REPEATCOUNT 100
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

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int core[] = {core00, core01, core02, core03, core10, core11, core12, core13, core20, core21, core22, core23, core30, core31, core32, core33};

    host_printf("k_passBroadcastNoWait: thread %i has debug[0] at %i\n", gid, pArgs->debug[0]);

    initLocal(vLocal, n, gid);

///     Uncomment these lines to see the vLocal array before any data is passed - make sure that you uncomment the barrier otherwise the other cores will start writing into the lcoal array before you have copied the data
//    if (gid == 6)
//        for (j=0; j< (CORECOUNT*n); j++)
//            g_debug[d++] = vLocal[j];
//            coprthr_barrier(0);

    firstI = gid * n;
    lastI = firstI + n;
    localCoreId = LOCAL_MEM_ADDRESS_BASE();

    STARTCLOCK(time_s);
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
    host_printf("Pass_Broardcast(pass up): Core with rank: %i, total time :%i\n", gid, time_s - time_e);
    pArgs->debug[gid] = time_s - time_e;

/// testing - 1
    repeater = REPEATCOUNT;

    STARTCLOCK(time_s);
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
    host_printf("Pass_Broardcast(pass 0 to 15): Core with rank: %i, total time :%i\n", gid, time_s - time_e);
    pArgs->debug[gid+16] = time_s - time_e;

/// testing - 2 4 way map
    repeater = REPEATCOUNT;

    STARTCLOCK(time_s);
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
    host_printf("Pass_Broardcast(4 way map): Core with rank: %i, total time :%i\n", gid, time_s - time_e);
    pArgs->debug[gid+32] = time_s - time_e;

/// Uncomment to use g_debug to show the final array contents
//                    if (gid == 6)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            g_debug[d++] = vLocal[j];

}

