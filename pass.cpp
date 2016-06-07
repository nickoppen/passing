#include <iostream>
#include <fstream>
#include <ctime>

extern "C" {
#include <coprthr.h>
#include <coprthr_cc.h>
#include <coprthr_thread.h>
#include <coprthr_mpi.h>
}

#include "passing.h"

#define ECORES 16    /// Not great - CORECOUNT is defined as the same thing in ringTopo16.c

//using namespace std;

int main()
{
    int i;    /// Uncomment when using debug
    int  n;
    clock_t tstart, tend;
    std::filebuf fbuf;
    fbuf.open("pass.csv", std::ios::out);
    std::ostream fout(&fbuf);
//    char strInfo[128];
    int host_debug[1024];
    pass_args args;

	int h_epip = coprthr_dopen(COPRTHR_DEVICE_E32,COPRTHR_O_THREAD);
	std::cout << "h_epip=" << h_epip << "\n";
	if (h_epip<0)
	{
        std::cout << "device open failed\n";
        exit(0);
    }

	coprthr_program_t prg;
	prg = coprthr_cc_read_bin("./passing.e32", 0);
	coprthr_sym_t thr_mpiPassUni = (coprthr_sym_t)coprthr_getsym(prg,"k_mpiPassUni");
	std::cout << "prg=" << prg << " thr mpiPassUni=" << thr_mpiPassUni  << "\n";
//   cl_kernel krnUni = clsym(stdacc, openHandle, "k_passUni", CLLD_NOW);
//   cl_kernel krnMPIUni = clsym(stdacc, openHandle, "k_mpiPassUni", CLLD_NOW);
//   cl_kernel krnMulti = clsym(stdacc, openHandle, "k_passMulti", CLLD_NOW);
//   cl_kernel krnBroadcast = clsym(stdacc, openHandle, "k_passBroadcastWait", CLLD_NOW);
//   cl_kernel krnNoWait = clsym(stdacc, openHandle, "k_passBroadcastNoWait", CLLD_NOW);


    for (n=1; n<=16; n++)
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
       std::cout << "about to malloc dev memory - MPI Unicast\n";
       coprthr_mem_t dev_debug = (coprthr_mem_t)coprthr_dmalloc(h_epip, 1024*sizeof(int), 0);             /// Allocate some space on the epiphany for debug
       std::cout << "about to write dev memory - MPI Unicast\n";
       coprthr_dwrite(h_epip, dev_debug, 0, host_debug, 1024*sizeof(int),COPRTHR_E_WAIT);    /// Initialise the debug memory
       std::cout << "written dev memory - MPI Unicast\n";

       tstart = clock();
	   coprthr_mpiexec(h_epip, ECORES, thr_mpiPassUni, &args, sizeof(args), 0);
       tend = clock();

       std::cout << "forked - MPI Unicast\n";

       fout << n << "," << "mpi unicast," << (tend - tstart) << "\n";

       coprthr_dread(h_epip, dev_debug, 0, host_debug, 1024*sizeof(int),COPRTHR_E_WAIT);


      /// Uncomment to use debug as output
      i = 0;
        while (i<1024)
        {
            if (host_debug[i] != -1)
            {
                fout << host_debug[i++];
                if ((i%(16*n)) == 0)
                    fout << std::endl;
                else
                    fout << ",";
            }
            else
                break;
        }
        fout << std::endl;
        fout.flush();


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

	coprthr_dclose(h_epip);
    fbuf.close();
    return 0;
}
