#include "ringTopo16.c"

__kernel void k_pass(__global int n, __global int method, __global int * debug)
{
    unsigned int gid_next, gid = get_global_id(0);
    unsigned int NEXT, PREV, ringIndex, gidOrder[CORECOUNT];
    unsigned int remoteMemBase;
    unsigned int i, j;
    unsigned int d = 0;
    unsigned int firstI, lastI;
    unsigned int magnitude, magMax;
    int nextGidIndex;
    unsigned int  repeater = 100000;  /// add to the work load a litte

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


    firstI = gid * n;                       /// the data to be passed is based on the core's global_id. This favours broadcast which is just iterates through the global ids writing to all. Unicast and Multicast are more complex because they
    lastI = firstI + n;                     /// write to the cores neighbour(s) which may not have a gid + or - 1. Therefore the gidOrder array must be looked up to see where the data came from.
    for (i=firstI; i<lastI; i++)
        vLocal[i] = gid + 100;            /// ADD 100 to distinguish the initialised value from the default value which is 0

    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

    while (repeater--)
    {
        if (method != 0)
        {
            initRing(&NEXT, &PREV, &ringIndex, gidOrder);       /// gidOrder[ringIndex] == gid;

            ///
            /// Unicast
            ///
            if (method == 1)
            {
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
            else
            {
                ///
                /// Multicast
                ///
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
        else
        {
//                    if (gid == 6)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            debug[d++] = vLocal[j];
//
//                    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

            ///
            /// Broadcast: transmit the node values calculated here to all other cores.
            ///
            gid_next = (gid == (CORECOUNT - 1)) ? 0 : gid +1;

            while (gid_next != gid)
            {
                remoteMemBase = LOCAL_MEM_ADDRESS_BASE(gid_next);
                for (i=firstI; i < lastI; i++)
                    *(int *)NEIGHBOUR_LOC(remoteMemBase, vLocal,  i, (sizeof(int))) = vLocal[i];
                gid_next = (gid_next == CORECOUNT - 1) ? 0 : gid_next + 1;
   ///             barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

    /// Uncomment to use debug as output
//                    if (gid == 6)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            debug[d++] = vLocal[j];
            }
        }
    }
}
