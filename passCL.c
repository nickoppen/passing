#include "ringTopo16.c"
#include "passing.h"
#include "timer.c"
//#include <coprthr.h>
//#include <coprthr2.h>
#include <coprthr_mpi.h>

#include "esyscall.h"

#include <host_stdio.h>

#define REPEATCOUNT 100000
#define MPI_BUF_SIZE 1024

/// global for debugging
int _debugSpace[4];


//__kernel void k_mpiPassUni(void * g_args)
void __entry k_mpiPassUni(void * g_args)
{
    pass_args * args = (pass_args *)g_args;
    int buf[DEBUG_BUFFER];
    unsigned int gid = coprthr_get_thread_id();
    int rank, size;
    int left, right;
    int rankNext, inmsg, outmsg, tag = 1, mpi_err;
    int msg_source;
    MPI_Status mpi_status;
    unsigned int d = 0;

//    e_dma_copy(buf, args->debug, DEBUG_BUFFER*sizeof(int));
    MPI_Status status;
    MPI_Init(0, MPI_BUF_SIZE);
    MPI_Comm comm = MPI_COMM_THREAD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    MPI_Cart_shift(comm, 0, 1, &left, &right);
    host_printf("host: %i - after Cart Shift, right is %i, left is %i\n", rank, right, left);

    d = rank * 5;

//    host_printf("k_mpiPassUni: thread %i has d at %i, debug[d] is %i, size is %i\n", gid, d, args->debug[d], size);

//    args->debug[d++] = gid+1;
//    args->debug[d++] = rank+16;
//    args->debug[d++] = size+32;
//    args->debug[d++] = left;
//    args->debug[d++] = right;

//    rankNext = (rank < (size-1)) ? rank + 1 : 0;
    outmsg = rank; /// start by passing the node's own rank
    do
    {
//        MPI_Send(&inmsg, 1, MPI_INT, rankNext, comm, tag);          /// pass on the message just received.
//        MPI_Recv(&inmsg, 1, MPI_INT, msg_source, tag, comm, &mpi_status);
        host_printf("core %i with rank: %i, sending message: %i to left: %i from right: %i\n", gid, rank, outmsg, left, right);
        mpi_err = MPI_Sendrecv_replace(&outmsg, 1, MPI_INT, left, 1, right, 1, comm, &mpi_status);
//        mpi_err = MPI_Sendrecv(&outmsg, 1, MPI_INT, left, 1, &inmsg, 1, MPI_INT, right, 1, comm, &mpi_status);
        host_printf("core with rank: %i, got a message: %i, err is %i\n", rank, outmsg, mpi_err);
//        outmsg = inmsg;   // pass on the incoming message (for MPI_Sendrecv)
    } while (outmsg != rank);

phalt();

    MPI_Finalize();
}

void initLocal(int * vLocal, int n, unsigned int base)
{
    unsigned int firstI, lastI, i;

    firstI = base * n;                       /// the data to be passed is based on the core's global_id. This favours broadcast which is just iterates through the global ids writing to all. Unicast and Multicast are more complex because they
    lastI = firstI + n;                     /// write to the cores neighbour(s) which may not have a gid + or - 1. Therefore the gidOrder array must be looked up to see where the data came from.
    for (i=firstI; i<lastI; i++)
        vLocal[i] = base + 100;            /// ADD 100 to distinguish the initialised value from the default value which is 0

}



///
/// Unicast
///

//__kernel void k_passUni(__global int g_n, __global int * g_debug)
void __entry k_passUni(pass_args * pArgs)
{
    unsigned int gid = coprthr_get_thread_id();
    unsigned int NEXT, PREV, ringIndex, gidOrder[CORECOUNT];
    unsigned int i, j, n;
    unsigned int d = 0;
    unsigned int firstI, lastI;
    unsigned int magnitude;
    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a litte

    pass_args * args = (pass_args*)pArgs; /// local copy of g_n
    n = args->n;

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    initRing(&NEXT, &PREV, &ringIndex, gidOrder);       /// gidOrder[ringIndex] == gid;
    initLocal(vLocal, n, ringIndex);                    /// the global_id has been replaced by the ringIndex as the primary "logical" location

//            coprthr_barrier(0);
    d = gid * 5;
    pArgs->debug[d] = gid;
//    host_printf("k_PassUni: thread %i has d at %i, debug at d is %i\n", gid, d, pArgs->debug[d]);
    host_printf("k_PassUni: thread %i has d at %i\n", gid, d);
    pArgs->debug[d] = gid;

    while (repeater--)
    {
        magnitude = ringIndex;      /// magnitude is the distance away from the starting point which is ringIndex
        do
        {
            firstI = magnitude * n;
            lastI = firstI + n;
            for (i=firstI; i<lastI; i++)
                *(int*)NEIGHBOUR_LOC(NEXT, vLocal, i, sizeof(int)) = vLocal[i];

            coprthr_barrier(0);

/// Uncomment to use g_debug as output - only do this if repeater == 1 (otherwise you will run out of room in the g_debug array)
//                    if (gid == 10)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            g_debug[d++] = vLocal[j];

            magnitude = (magnitude == 0) ? 15 : magnitude - 1;
        }
        while (magnitude != ringIndex);
    }
}

///
/// Multicast
///

//__kernel void k_passMulti(__global int g_n, __global int * g_debug)
void __entry k_passMulti(pass_args * pArgs)
{
    unsigned int gid = coprthr_get_thread_id();
    unsigned int NEXT, PREV, ringIndex, gidOrder[CORECOUNT];
    unsigned int i, j;
    unsigned int d = 0;
    unsigned int firstI, lastI;
    unsigned int magnitude, magMax;
    int nextIndex;
    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a litte

    int n = pArgs->n; /// local copy of g_n

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    initRing(&NEXT, &PREV, &ringIndex, gidOrder);       /// gidOrder[ringIndex] == gid;
    initLocal(vLocal, n, ringIndex);

    host_printf("k_PassMulti: thread %i has d at %i\n", gid, d);
    coprthr_barrier(0);

    while (repeater--)
    {
        magnitude = 0;
        magMax = CORECOUNT / 2; /// sending two sets of numbers at the same time
        while (magnitude < magMax)
        {
            nextIndex = (ringIndex + magnitude);
            firstI = ((nextIndex >= CORECOUNT) ? nextIndex - CORECOUNT : nextIndex) * n;
            lastI = firstI + n;

            for (i = firstI; i < lastI; i++)
                *(int*)NEIGHBOUR_LOC(PREV, vLocal, i, sizeof(int)) = vLocal[i];

            nextIndex = (ringIndex - magnitude);
            firstI = ((nextIndex < 0) ? nextIndex + CORECOUNT : nextIndex) * n;
            lastI = firstI + n;

            for (i = firstI; i < lastI; i++)
                *(int*)NEIGHBOUR_LOC(NEXT, vLocal, i, sizeof(int)) = vLocal[i];

            coprthr_barrier(0);
            magnitude++;

/// Uncomment to use g_debug as output
//                    if (gid == 10)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            g_debug[d++] = vLocal[j];
        }
    }
}

//__kernel void k_mpiPassMulti(__global int g_n, __global int * g_debug)
void __entry k_mpiPassMulti(pass_args * pArgs)
{
    int rank, size;
    int left, right;

    MPI_Status status;
    MPI_Init(0,MPI_BUF_SIZE);
    MPI_Comm comm = MPI_COMM_THREAD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    MPI_Cart_shift(comm, 0, 1, &left, &right);

    host_printf("k_mpiPassMulti: thread  has d at \n");

    MPI_Finalize();
}


///
/// Broadcast: transmit the node values calculated here to all other cores without waiting between sends.
///


//__kernel void k_passBroadcastNoWait(__global int g_n, __global int * g_debug)
void __entry k_passBroadcastNoWait(pass_args * pArgs)
{
    unsigned int gid = coprthr_get_thread_id();
    unsigned int localCoreId;
    unsigned int i, j, coreI;
    unsigned int d = 0;
    unsigned int firstI, lastI;
    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a litte
    int n = pArgs->n; /// local copy of g_n

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int core[] = {core00, core01, core02, core03, core10, core11, core12, core13, core20, core21, core22, core23, core30, core31, core32, core33};

    host_printf("k_passBroadcastNoWait: thread %i has d at %i\n", gid, d);

    initLocal(vLocal, n, gid);

///     Uncomment these lines to see the vLocal array before any data is passed - make sure that you uncomment the barrier otherwise the other cores will start writing into the lcoal array before you have copied the data
//    if (gid == 6)
//        for (j=0; j< (CORECOUNT*n); j++)
//            g_debug[d++] = vLocal[j];
//            coprthr_barrier(0);

    firstI = gid * n;
    lastI = firstI + n;
    localCoreId = LOCAL_MEM_ADDRESS_BASE();

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

/// Uncomment to use g_debug to show the final array contents
//                    if (gid == 6)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            g_debug[d++] = vLocal[j];

}

//__kernel void k_mpiBroadcast(__global int g_n, __global int * g_debug)
void __entry k_mpiBroadcast(pass_args * pArgs)
{
    int rank, size;
    int left, right;
    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a litte
    unsigned int execTime;

    MPI_Status status;
    MPI_Init(0,MPI_BUF_SIZE);
    MPI_Comm comm = MPI_COMM_THREAD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    MPI_Cart_shift(comm, 0, 1, &left, &right);

    init_timer();
    init_clock();

    host_printf("k_mpiPassMulti: thread  has d at \n");
    while (repeater--)
    {
    }

    execTime = get_clock();     /// get the number of clock ticks


    MPI_Finalize();
}
