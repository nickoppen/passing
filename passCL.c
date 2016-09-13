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
#define REPEATCOUNT 1
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
/// Unicast
///
void __entry k_mpiPassUni(void * g_args)
{
    pass_args * args = (pass_args *)g_args;
    int n = args->n;
    unsigned int gid = coprthr_get_thread_id();
    int rank, size;
    int rankNext, rankPrev, tag = 1, mpi_err;
    int rankOrder[CORECOUNT];
    int ringIndex;
    int newI;
//    int * outmsg, * inmsg;    //non replace
    int outmsg[CORECOUNT];      // REPLACE
    int msg_source, i, msg_sourceIndex;
    MPI_Status mpi_status;
    unsigned int j, d = 0;
    unsigned time_e, time_s, idle_e, idle_s;

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    MPI_Status status;
    MPI_Init(0, MPI_BUF_SIZE);
    MPI_Comm comm = MPI_COMM_THREAD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    mpi_initRing(rank, &rankNext, &rankPrev, &ringIndex, rankOrder);        /// the data goes to renkNext and comes from rankPrev

    initLocal(vLocal, n, rank);                    /// the global_id has been replaced by the rank as the primary "logical" location
//    d = rank * 5;

    host_printf("k_mpiPassUni: thread %i has d at %i, debug[d] is %i, size is %i\n", gid, d, args->debug[d], size);

//         copy in copy out because non replace is not working
    memcpy(outmsg, vLocal + (n * rank), n * sizeof(int));    /// copy in the local core's data to start the process
//    phalt();
    STARTCLOCK(time_s);

    for (i = 0; i < CORECOUNT; i++)
    {
//        if (rank==5) host_printf("core %i with rank: %i, sending message: %i, %i, %i, %i to next: %i from prev: %i, src is %i\n", gid, rank, outmsg[0], outmsg[1], outmsg[2], outmsg[3],  rankNext, rankPrev, msg_source);

        mpi_err = MPI_Sendrecv_replace(outmsg, n, MPI_INT, rankNext, 1, rankPrev, 1, comm, &mpi_status);
        msg_sourceIndex = ringIndex - (i+1);        /// getting data from previous elements (a negative direction) in the ring and passing it onwards (a positive direction)
        msg_source = rankOrder[(msg_sourceIndex >= 0) ? msg_sourceIndex : msg_sourceIndex + CORECOUNT];
        memcpy(vLocal + (n * msg_source), outmsg, n * sizeof(int));   /// keep thing incoming data
//        memcpy(_debugSpace, outmsg, n * sizeof(int));   /// keep thing incoming data
///        if (rank==9) host_printf("core with rank: %i, got a message: %i, %i, %i, index of sender: %i, rank of sender: %i, vLocal offset: %i\n", rank, outmsg[0], outmsg[1], outmsg[2], (msg_sourceIndex >= 0) ? msg_sourceIndex : msg_sourceIndex + CORECOUNT, msg_source, n*msg_source);
//phalt();

/// send the data from vLocal and insert the incoming data directly into vlocal
//        LOOPINGDECREMENT(newI, ringIndex, i);
//        outmsg = vLocal + (sizeof(int) * (n * rankOrder[newI]));       /// we pass on the data from the down stream core
//        LOOPINGDECREMENT(newI, ringIndex, (i + 1));
//        inmsg = vLocal + (sizeof(int) * (n * rankOrder[newI]));     /// we receive data from the one before the previous core
//        host_printf("core with rank: %i, ringIndex: %i, iteration: %i, msg_source: %i, in: %i\n", rank, ringIndex, i, msg_source, rankOrder[newI]);

//        mpi_err = MPI_Sendrecv(outmsg, n, MPI_INT, rankNext, 1, inmsg, n, MPI_INT, rankPrev, 1, comm, &mpi_status);

/// Uncomment to use pArgs->debug as output - only do this if repeater == 1 (otherwise you will run out of room in the g_debug array)
/*        if (rank == 1)
        {
            host_printf("1-");
            for (j=0; j< (CORECOUNT*n); j++)
            {
//                args->debug[d++] = vLocal[j];
                host_printf("%i,", vLocal[j]);
            }
            host_printf("\n");
        }*/
    }
    STOPCLOCK(time_e);
    host_printf("MPI_Pass_Uni: Core with rank: %i, total time :%i\n", rank, time_s - time_e);

    MPI_Finalize();
}



void __entry k_passUni(pass_args * pArgs)
{
    unsigned int gid = coprthr_get_thread_id();
    unsigned int NEXT, PREV, ringIndex, gidOrder[CORECOUNT];
    unsigned int i, j, n;
    unsigned int d = 0;
    unsigned int firstI, lastI;
    unsigned int magnitude;
    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a little
    unsigned time_e, time_s, idle_e, idle_s;

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

    STARTCLOCK(time_s);
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

/// Uncomment to use pArgs->debug as output - only do this if repeater == 1 (otherwise you will run out of room in the g_debug array)
                    if (gid == 10)
                    {
                        host_printf("Core: 10 - ");
                        for (j=0; j< (CORECOUNT*n); j++)
                        {
                            pArgs->debug[d++] = vLocal[j];
                            host_printf(" %i,", vLocal[j]);
                        }
                        host_printf("\n");
                    }

            magnitude = (magnitude == 0) ? 15 : magnitude - 1;
        }
        while (magnitude != ringIndex);
    }
    STOPCLOCK(time_e);
    host_printf("Pass_Uni: Core with rank: %i, total time :%i\n", gid, time_s - time_e);

}


///
/// Multicast
///
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
    unsigned time_e, time_s, idle_e, idle_s;

    int n = pArgs->n; /// local copy of g_n

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    initRing(&NEXT, &PREV, &ringIndex, gidOrder);       /// gidOrder[ringIndex] == gid;
    initLocal(vLocal, n, ringIndex);

    host_printf("k_PassMulti: thread %i has d at %i\n", gid, d);
    coprthr_barrier(0);

    STARTCLOCK(time_s);
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
            if (gid == 10)
            {
                host_printf("Core: 10 - ");
                for (j=0; j< (CORECOUNT*n); j++)
                {
                    pArgs->debug[d++] = vLocal[j];
                    host_printf(" %i,", vLocal[j]);
                }
                host_printf("\n");
            }
        }
    }
    STOPCLOCK(time_e);
    host_printf("Pass_Multi: Core with rank: %i, total time :%i\n", gid, time_s - time_e);

}

void __entry k_mpiPassMulti(pass_args * pArgs)
{
    int n, magnitude, magMax;
    int rank, size;
    int rankNext, rankPrev, tag = 1, mpi_err;
    int rankOrder[CORECOUNT];
    int ringIndex;
    int * outmsg_up, * inmsg_up;
    int * outmsg_down, * inmsg_down;
    int newI;
    unsigned time_e, time_s, idle_e, idle_s;

    MPI_Status status;
    MPI_Init(0,MPI_BUF_SIZE);
    MPI_Comm comm = MPI_COMM_THREAD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    mpi_initRing(rank, &rankNext, &rankPrev, &ringIndex, rankOrder);
//    host_printf("host: %i - after initRing, next is %i, prev is %i\n", rank, rankNext, rankPrev);

    n = pArgs->n;
    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    initLocal(vLocal, n, rank);                    /// the global_id has been replaced by the rank as the primary "logical" location

    host_printf("k_mpiPassMulti: thread  has d at \n");

    magMax = CORECOUNT / 2;
    magnitude = 0;

    STARTCLOCK(time_s);
    while (magnitude < magMax)
    {
//        if (rank==5) host_printf("core %i with rank: %i, sending message: %i, %i, %i, %i to next: %i from prev: %i, src is %i\n", gid, rank, outmsg[0], outmsg[1], outmsg[2], outmsg[3],  rankNext, rankPrev, msg_source);

/// need to check boundry conditions

        LOOPINGINCREMENT(newI, ringIndex, magnitude);    /// ringIndex + magnitude looped if over 15
        outmsg_down = vLocal + (sizeof(int) * (n * rankOrder[newI]));
        LOOPINGINCREMENT(newI, ringIndex, (magnitude + 1));    /// ringIndex + magnitude + 1 looped if over 15
        inmsg_down = vLocal + (sizeof(int) * (n * rankOrder[newI]));
        LOOPINGDECREMENT(newI, ringIndex, magnitude);
        outmsg_up = vLocal + (sizeof(int) * (n * rankOrder[newI]));
        LOOPINGDECREMENT(newI, ringIndex, (magnitude + 1));
        inmsg_up = vLocal + (sizeof(int) * (n * rankOrder[newI]));

//        mpi_err = MPI_Sendrecv(outmsg_up, n, MPI_INT, rankNext, 1, inmsg_up, n, MPI_INT, rankPrev, 1, comm, &mpi_status);
//        mpi_err = MPI_Sendrecv(outmsg_down, n, MPI_INT, rankPrev, 1, inmsg_down, n, MPI_INT, rankNext, 1, comm, &mpi_status);

/// Uncomment to use pArgs->debug as output - only do this if repeater == 1 (otherwise you will run out of room in the g_debug array)
/*        if (rank == 1)
        {
            host_printf("1-");
            for (j=0; j< (CORECOUNT*n); j++)
            {
//                args->debug[d++] = vLocal[j];
                host_printf("%i,", vLocal[j]);
            }
            host_printf("\n");
        }*/
        magnitude++;
    }
    STOPCLOCK(time_e);
    host_printf("MPI_Pass_Multi: Core with rank: %i, total time :%i\n", rank, time_s - time_e);

    MPI_Finalize();
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
    host_printf("Pass_Broardcast: Core with rank: %i, total time :%i\n", gid, time_s - time_e);


/// Uncomment to use g_debug to show the final array contents
//                    if (gid == 6)
//                        for (j=0; j< (CORECOUNT*n); j++)
//                            g_debug[d++] = vLocal[j];

}

void __entry k_mpiPassBroadcast(pass_args * pArgs)
{
    int i, n, step;
    int rank, size;
    int * pBuffer;
    unsigned int  repeater = REPEATCOUNT;  /// add to the work load a litte
    unsigned int execTime;
    unsigned time_e, time_s, idle_e, idle_s;

    MPI_Status status;
    MPI_Init(0,MPI_BUF_SIZE);
    MPI_Comm comm = MPI_COMM_THREAD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int vLocal[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    initLocal(vLocal, n, rank);                    /// the global_id has been replaced by the rank as the primary "logical" location


    host_printf("k_mpiPassMulti: thread  has d at \n");
    pBuffer = vLocal;
    step = n * sizeof(int);
    STARTCLOCK(time_s);
    while (repeater--)
    {
        for (i=0; i < size; i++)
        {
            //coprthr_mpi_Bcast(pBuffer, n, MPI_INT, i, comm); not implemented yet
            pBuffer += step;
        }
    }
    STOPCLOCK(time_e);
    host_printf("MPI_Pass_Broardcast: Core with rank: %i, total time :%i\n", rank, time_s - time_e);

    MPI_Finalize();
}
