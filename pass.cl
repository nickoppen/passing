#include "ringTopo16.c"
#define REPEATCOUNT 100000

void initLocal(int * vLocal, int n, unsigned int gid)
{
    unsigned int firstI, lastI, i;

    firstI = gid * n;                       /// the data to be passed is based on the core's global_id. This favours broadcast which is just iterates through the global ids writing to all. Unicast and Multicast are more complex because they
    lastI = firstI + n;                     /// write to the cores neighbour(s) which may not have a gid + or - 1. Therefore the gidOrder array must be looked up to see where the data came from.
    for (i=firstI; i<lastI; i++)
        vLocal[i] = gid + 100;            /// ADD 100 to distinguish the initialised value from the default value which is 0

}

        ///
        /// Unicast
        ///

__kernel void k_passUni(__global int g_n, __global int * g_debug)
{
    unsigned int gid = get_global_id(0);
    unsigned int NEXT, PREV, ringIndex, gidOrder[CORECOUNT];
    unsigned int i, j;
    unsigned int d = 0;
    unsigned int firstI, lastI;
    unsigned int magnitude;
    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a litte
    int n = g_n; /// local copy of g_n

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    initLocal(vLocal, n, gid);

    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

    while (repeater--)
    {
        initRing(&NEXT, &PREV, &ringIndex, gidOrder);       /// gidOrder[ringIndex] == gid;

        magnitude = ringIndex;      /// magnitude is the distance away from the starting point which is ringIndex
        do
        {
            firstI = gidOrder[magnitude] * n;
            lastI = firstI + n;
            for (i=firstI; i<lastI; i++)
                *(int*)NEIGHBOUR_LOC(NEXT, vLocal, i, sizeof(int)) = vLocal[i];

            barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

/// Uncomment to use debug as output - only do this if repeater == 1 (otherwise you will run out of room in the debug array)
//                    if (gid == 0)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            debug[d++] = vLocal[j];

            magnitude = (magnitude == 0) ? 15 : magnitude - 1;
        }  while (magnitude != ringIndex);
    }
}


        ///
        /// Multicast
        ///

__kernel void k_passMulti(__global int g_n, __global int * g_debug)
{
    unsigned int gid = get_global_id(0);
    unsigned int NEXT, PREV, ringIndex, gidOrder[CORECOUNT];
    unsigned int i, j;
    unsigned int d = 0;
    unsigned int firstI, lastI;
    unsigned int magnitude, magMax;
    int nextGidIndex;
    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a litte
    int n = g_n; /// local copy of g_n

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    initLocal(vLocal, n, gid);

    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

    initRing(&NEXT, &PREV, &ringIndex, gidOrder);       /// gidOrder[ringIndex] == gid;

    while (repeater--)
    {
        magnitude = 0;
        magMax = CORECOUNT / 2; /// sending two sets of numbers at the same time
        while (magnitude < magMax)
        {
            nextGidIndex = (ringIndex + magnitude);
            firstI = gidOrder[(nextGidIndex >= CORECOUNT) ? nextGidIndex - CORECOUNT : nextGidIndex] * n;
            lastI = firstI + n;

            for (i = firstI; i < lastI; i++)
                *(int*)NEIGHBOUR_LOC(PREV, vLocal, i, sizeof(int)) = vLocal[i];

            nextGidIndex = (ringIndex - magnitude);
            firstI = gidOrder[(nextGidIndex < 0) ? nextGidIndex + CORECOUNT : nextGidIndex] * n;
            lastI = firstI + n;

            for (i = firstI; i < lastI; i++)
                *(int*)NEIGHBOUR_LOC(NEXT, vLocal, i, sizeof(int)) = vLocal[i];

            barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
            magnitude++;

/// Uncomment to use debug as output
//                    if (gid == 0)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            debug[d++] = vLocal[j];
        }
    }
}

        ///
        /// Broadcast: transmit the node values calculated here to all other cores.
        ///


__kernel void k_passBroadcastWait(__global int g_n, __global int * g_debug)
{
    unsigned int gid_next, gid = get_global_id(0);
    unsigned int remoteMemBase;
    unsigned int i, j;
    unsigned int d = 0;
    unsigned int firstI, lastI;
    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a litte
    int n= g_n; /// local copy of g_n

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    initLocal(vLocal, n, gid);
    firstI = gid * n;
    lastI = firstI + n;

    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

    while (repeater--)
    {
//                    if (gid == 6)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            debug[d++] = vLocal[j];
//
//                    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

        gid_next = (gid == (CORECOUNT - 1)) ? 0 : gid +1;

        while (gid_next != gid)
        {
            remoteMemBase = LOCAL_MEM_ADDRESS_BASE(gid_next);
            for (i=firstI; i < lastI; i++)
                *(int *)NEIGHBOUR_LOC(remoteMemBase, vLocal,  i, (sizeof(int))) = vLocal[i];
            gid_next = (gid_next == CORECOUNT - 1) ? 0 : gid_next + 1;
            barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

/// Uncomment to use debug as output
//                    if (gid == 6)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            debug[d++] = vLocal[j];
        }
    }
}

        ///
        /// Broadcast: transmit the node values calculated here to all other cores without waiting between sends.
        ///


__kernel void k_passBroadcastNoWait(__global int g_n, __global int * g_debug)
{
    unsigned int gid = get_global_id(0);
    unsigned int localCoreId;
    unsigned int i, j, coreI;
    unsigned int d = 0;
    unsigned int firstI, lastI;
    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a litte
    int n = g_n; /// local copy of g_n

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int core[] = {core00, core01, core02, core03, core10, core11, core12, core13, core20, core21, core22, core23, core30, core31, core32, core33};

    initLocal(vLocal, n, gid);

    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

    firstI = gid * n;
    lastI = firstI + n;
    localCoreId = LOCAL_MEM_ADDRESS_BASE(gid);

    while (repeater--)
    {
        coreI = (gid == (CORECOUNT - 1)) ? 0 : gid + 1;
        while (coreI != gid)
        {
            if (core[coreI] != localCoreId)
                for (i=firstI; i < lastI; i++)
                    *(int *)NEIGHBOUR_LOC(core[coreI], vLocal,  i, (sizeof(int))) = vLocal[i];
            coreI = (coreI == (CORECOUNT - 1)) ? 0 : coreI++;
        }
    }
}
