#include "ringTopo16.c"

__kernel void k_pass(__global int n, __global int method, __global int * debug)
{
    unsigned int gid_next, gid = get_global_id(0);
    unsigned int NEXT, PREV, ringIndex, myNeighbour, gidOrder[CORECOUNT];
    unsigned int remoteMemBase;
    unsigned int i;
    unsigned int j, d = 0;
    unsigned int firstI, lastI;
    unsigned int magnitude, magMax;
    int breaker;

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


    firstI = gid * n;
    lastI = firstI + n;
    for (i=firstI; i<lastI; i++)
        vLocal[i] = gid + 100;            /// ADD 100 to distinguish the initialised value from the default value which is 0

breaker = 8;
    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

    if (method != 0)
    {
        initRing(&NEXT, &PREV, &ringIndex, gidOrder);
        if (method == 1)
        {
            magnitude = ringIndex;      /// gidOrder[ringIndex] == gid;
            do
            {
                firstI = gidOrder[magnitude] * n;
                lastI = firstI + n;
                for (i=firstI; i<lastI; i++)
                    *(int*)NEIGHBOUR_LOC(NEXT, vLocal, i, sizeof(int)) = vLocal[i];

                barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

                    if (gid == 0)
                        for (j=0; j< (CORECOUNT*n); j++)
                            debug[d++] = vLocal[j];

//                if ((breaker--)<=0) break;

                magnitude = (magnitude == 0) ? 15 : magnitude - 1;
            }  while (magnitude != ringIndex);
        }
        else
        {
            for (i=firstI; i<lastI; i++)
            {
                *(int*)NEIGHBOUR_LOC(NEXT, vLocal, i, sizeof(int)) = vLocal[i];
                *(int*)NEIGHBOUR_LOC(PREV, vLocal, i, sizeof(int)) = vLocal[i];
            }
            barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

            magnitude = 1;
            magMax = CORECOUNT / 2; /// sending two sets of numbers at the same time
            while (magnitude < magMax)
            {
                if ((gid + magnitude) >= CORECOUNT)      /// run off the end of the array therefore start again from the bottom
                    firstI = CORECOUNT - ((gid * n) + (n * magnitude));
                else
                    firstI = (gid * n) + (n * magnitude);
                lastI = firstI + n;

                for (i = firstI; i < lastI; i++)
                    *(int*)NEIGHBOUR_LOC(PREV, vLocal, i, sizeof(int)) = vLocal[i];

                if ((gid - magnitude) < 0)
                    firstI = CORECOUNT + ((gid * n) - (n * magnitude));
                else
                    firstI = (gid * n) - (n * magnitude);
                lastI = firstI + n;

                for (i = firstI; i < lastI; i++)
                    *(int*)NEIGHBOUR_LOC(NEXT, vLocal, i, sizeof(int)) = vLocal[i];

                barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
                magnitude++;

                if ((breaker--)<=0) break;

                    if (gid == 0)
                        for (j=0; j< (CORECOUNT*n); j++)
                            debug[d++] = vLocal[j];
            }

        }
    }
    else
    {
        /// transmit the node values calculated here to all other cores.
        gid_next = (gid == (CORECOUNT - 1)) ? 0 : gid +1;
        while (gid_next != gid)
        {
            remoteMemBase = LOCAL_MEM_ADDRESS_BASE(gid_next);
            for (i=firstI; i < lastI; i++)
                *(int *)NEIGHBOUR_LOC(remoteMemBase, vLocal,  i, (sizeof(int))) = vLocal[i];
            gid_next = (gid_next == CORECOUNT - 1) ? 0 : gid_next + 1;
            barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

                if ((breaker--)<=0) break;
                if (gid == 0)
                    for (j=0; j< (CORECOUNT*n); j++)
                        debug[d++] = vLocal[j];
        }
    }

 //   if (gid == 0)
 //       for (j=0; j< 16; j++)
 //           debug[d++] = vLocal[j];

}
