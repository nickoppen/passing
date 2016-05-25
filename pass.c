//#include <iostream>
//#include <fstream>
//#include <ctime>
#include <stdio.h>

//extern "C" {
#include <coprthr.h>
#include <coprthr_cc.h>
#include <coprthr_thread.h>
#include <coprthr_mpi.h>
//}

#include "passing.h"

#define ECORES 16    /// Not great - CORECOUNT is defined as the same thing in ringTopo16.c

//using namespace std;

int main()
{
    int i;    /// Uncomment when using debug
    int  n;
    clock_t tstart, tend;
//    std::filebuf fbuf;
//    fbuf.open("pass.csv", std::ios::out);
//    std::ostream fout(&fbuf);
//    char strInfo[128];
    int host_debug[1024];
    pass_args args;

	int dd = coprthr_dopen(COPRTHR_DEVICE_E32,COPRTHR_O_THREAD);
//	std::cout << "dd=" << dd << "\n";
    printf("epiphany dev:%i\n", dd);
	if (dd<0)
	{
//        std::cout << "device open failed\n";
        printf("Device open failed.\n");
        exit(0);
    }

	coprthr_program_t prg = coprthr_cc_read_bin("./passing.e32", 0);
	coprthr_sym_t thr_passUni = (coprthr_sym_t)coprthr_getsym(prg,"k_passUni");
	coprthr_sym_t thr_mpiPassUni = (coprthr_sym_t)coprthr_getsym(prg,"k_mpiPassUni");
//	std::cout << "prg=" << prg << " thr mpiPassUni=" << thr_mpiPassUni  << "\n";
//   coprthr_sym_t krnUni = clsym(stdacc, openHandle, "k_passUni", CLLD_NOW);
//   coprthr_sym_t krnMPIUni = clsym(stdacc, openHandle, "k_mpiPassUni", CLLD_NOW);
   coprthr_sym_t krnMulti = (coprthr_sym_t)coprthr_getsym(prg, "k_passMulti");
//   coprthr_sym_t krnBroadcast = clsym(stdacc, openHandle, "k_passBroadcastWait", CLLD_NOW);
//   coprthr_sym_t krnNoWait = clsym(stdacc, openHandle, "k_passBroadcastNoWait", CLLD_NOW);


//    for (n=1; n<=16; n++)
    for (n=1; n<=1; n++)
    {
        //std::cout << "n is equal to: " << n << "\n";
        args.n = n;     /// passed to all kernals

/// unicast
/// Uncomment if using debug
//        for (i=0; i<1024; i++)
//            debug[i] = -1;
/*       clmsync(stdacc, 0, debug, CL_MEM_DEVICE|CL_EVENT_WAIT);

       tstart = clock();

       clforka(stdacc, 0, krnUni, &ndr, CL_EVENT_WAIT, n, debug);
       cout << "forked - Unicast\n";
       tend = clock();

       fout << n << "," << "unicast," << (tend - tstart) << endl;
       clmsync(stdacc, 0, debug, CL_MEM_HOST|CL_EVENT_WAIT);

      /// Uncomment to use debug as output
      i = 0;
        while (i<1024)
        {
            if (debug[i] != -1)
            {
                fout << debug[i++];
                if ((i%(16*n)) == 0)
                    fout << endl;
                else
                    fout << ",";
            }
            else
                break;
        }
        fout << endl;  */
//        fout.flush();

/// MPI unicast
/// Uncomment if using debug
       for (i = 0; i < 1024; i++) host_debug[i] = -1;                                       /// initialise the host debug space

//       std::cout << "about to malloc dev memory - MPI Unicast\n";
        printf("About to allocate device shared memory.\n");
       coprthr_mem_t dev_debug = (coprthr_mem_t)coprthr_dmalloc(dd, 1024*sizeof(int), 0);             /// Allocate some space on the epiphany for debug

//       std::cout << "about to write dev memory - MPI Unicast\n";
       printf("About to write to device shared memory.\n");
       coprthr_dwrite(dd, dev_debug, 0, host_debug, 1024*sizeof(int),COPRTHR_E_WAIT);    /// Initialise the debug memory
//       std::cout << "written dev memory - MPI Unicast\n";
        args.debug = (int*)dev_debug;

       tstart = clock();
	   coprthr_mpiexec(dd, ECORES, thr_mpiPassUni, &args, sizeof(args), 0);
       tend = clock();
	   coprthr_mpiexec(dd, ECORES, krnMulti, &args, sizeof(args), 0);

//       std::cout << "forked - MPI Unicast\n";
       printf("forked - MPI Unicast. Exec time was: %i\n", (int)(tend - tstart));

//       fout << n << "," << "mpi unicast," << (tend - tstart) << "\n";

       coprthr_dread(dd, dev_debug, 0, host_debug, 1024*sizeof(int),COPRTHR_E_WAIT);
        printf("Read return data from shared memory\n");

      /// Uncomment to use debug as output
      i = 0;
        while (i<1024)
        {
//            if (host_debug[i] != -1)
            if (i < (16*5))
            {
//                fout << host_debug[i++];
                printf("%d, ", host_debug[i++]);
                if ((i%(16*n)) == 0)
//                    fout << std::endl;
                    printf("\n");
                else
                    printf("'");
//                    fout << ",";
            }
            else
                break;
        }
        printf("\n");
//        fout << std::endl;
//        fout.flush();


/// multicast

/// Uncomment if using debug
//        for (i=0; i<1024; i++)
//            debug[i] = -1;
//       clmsync(stdacc, 0, debug, CL_MEM_DEVICE|CL_EVENT_WAIT);
//
//       tstart = clock();
//       clforka(stdacc, 0, krnMulti, &ndr, CL_EVENT_WAIT, n, debug);
//       cout << "forked - Multicast\n";
//       tend = clock();
//
//       fout << n << "," << "multicast," << (tend - tstart) << endl;
//       clmsync(stdacc, 0, debug, CL_MEM_HOST|CL_EVENT_WAIT);

    /// Uncomment to use debug as output
 /*       i = 0;
        while (i<1024)
        {
            if (debug[i] != -1)
            {
                fout << debug[i++];
                if ((i%(16*n)) == 0)
                    fout << endl;
                else
                    fout << ",";
            }
            else
                break;
        }
        fout << endl;  */
 //       fout.flush();

/// broardcast

/// Uncomment if using debug
//        for (i=0; i<1024; i++)
//            debug[i] = -1;
//       clmsync(stdacc, 0, debug, CL_MEM_DEVICE|CL_EVENT_WAIT);
//
//       tstart = clock();
//       clforka(stdacc, 0, krnBroadcast, &ndr, CL_EVENT_WAIT, n, debug);
//       cout << "forked - Broadcast\n";
//       tend = clock();
//
//       fout << n << "," << "broadcast," << (tend - tstart) << endl;
//       clmsync(stdacc, 0, debug, CL_MEM_HOST|CL_EVENT_WAIT);

    /// Uncomment to use debug as output
   /*     i = 0;
        while (i<1024)
        {
            if (debug[i] != -1)
            {
                fout << debug[i++];
                if ((i%(16*n)) == 0)
                    fout << endl;
                else
                    fout << ",";
            }
            else
                break;
        }
        fout << endl;
   */
//        fout.flush();


/// broardcast - No Wait

/// Uncomment if using debug
//        for (i=0; i<1024; i++)
//            debug[i] = -1;
//       clmsync(stdacc, 0, debug, CL_MEM_DEVICE|CL_EVENT_WAIT);
//
//       tstart = clock();
//       clforka(stdacc, 0, krnNoWait, &ndr, CL_EVENT_WAIT, n, debug);
//       tend = clock();
//       cout << "forked Broadcast - No wait.\n";
//       tend = clock();
//
//       fout << n << "," << "broadcastNoWait," << (tend - tstart) << endl;
//       clmsync(stdacc, 0, debug, CL_MEM_HOST|CL_EVENT_WAIT);

    /// Uncomment to use debug as output
    /// First Retrieve the debug output from the cores - TODO
   /*     i = 0;
        while (i<1024)
        {
            if (debug[i] != -1)
            {
                fout << debug[i++];
                if ((i%(16*n)) == 0)
                    fout << endl;
                else
                    fout << ",";
            }
            else
                break;
        }
        fout << endl;
   */
//         fout.flush();
    }

	coprthr_dclose(dd);
//    fbuf.close();
    return 0;
}
