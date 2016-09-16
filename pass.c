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
// broadcastStrat branch

int main()
{
    int i;    /// Uncomment when using debug
    int  n;
    clock_t tstart, tend;
//    std::filebuf fbuf;
//    fbuf.open("pass.csv", std::ios::out);
//    std::ostream fout(&fbuf);
//    char strInfo[128];
    int host_debug[DEBUG_BUFFER];
//    int * host_debug = (int*)malloc(DEBUG_BUFFER*sizeof(int));
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
//	coprthr_kernel_t thr_passUni = (coprthr_sym_t)coprthr_getsym(prg,"k_passUni");
//	coprthr_sym_t thr_mpiPassUni = (coprthr_sym_t)coprthr_getsym(prg,"k_mpiPassUni");
//   coprthr_sym_t thr_mpiMulti = clsym(stdacc, openHandle, "k_mpiPassMulti", CLLD_NOW);
//   coprthr_sym_t thr_Multi = (coprthr_sym_t)coprthr_getsym(prg, "k_passMulti");
//   coprthr_sym_t thr_mpiBroadcast = clsym(stdacc, openHandle, "k_mpiPassBroadcast", CLLD_NOW);
	coprthr_kernel_t thr_Broadcast = (coprthr_sym_t)coprthr_getsym(prg,"k_passBroadcast");

       coprthr_mem_t dev_debug = (coprthr_mem_t)coprthr_dmalloc(dd, DEBUG_BUFFER*sizeof(int), 0);             /// Allocate some space on the epiphany for debug
        for (i=0; i<DEBUG_BUFFER; i++)
            host_debug[i] = -1;
        for (i=0; i<32; i++)    /// testing
            host_debug[i] = 0;
        coprthr_dwrite(dd, dev_debug, 0, host_debug, DEBUG_BUFFER*sizeof(int), COPRTHR_E_WAIT);
        args.debug = coprthr_memptr(dev_debug, 0);

//    for (n=1; n<=16; n++)
    for (n=3; n<=3; n=n+2)
    {
        //std::cout << "n is equal to: " << n << "\n";
        args.n = n;     /// passed to all kernels

 /// unicast
/*
        printf("about to call Unipass.\n");
       tstart = clock();
//       coprthr_dexec(dd, ECORES, thr_passUni, &args, COPRTHR_E_WAIT);   // wait for the documentaiton for this call
        coprthr_mpiexec(dd, ECORES, thr_passUni, &args, sizeof(pass_args), 0);
       tend = clock();
       printf("forked - MPI Unicast. Exec time was: %i\n", (int)(tend - tstart));

//       fout << n << "," << "unicast," << (tend - tstart) << endl;
        coprthr_dread(dd, dev_debug, 0, host_debug, DEBUG_BUFFER*sizeof(int), COPRTHR_E_NOWAIT);

      /// Uncomment to use debug as output
      i = 0;
        while (i<DEBUG_BUFFER)
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
//        fout << endl;
//        fout.flush();
*/
/// MPI unicast
/// Uncomment if using debug
/*
       tstart = clock();
	   coprthr_mpiexec(dd, ECORES, thr_mpiPassUni, &args, sizeof(args), 0);
       tend = clock();

//       std::cout << "forked - MPI Unicast\n";
       printf("forked - MPI Unicast. Exec time was: %i\n", (int)(tend - tstart));

//       fout << n << "," << "mpi unicast," << (tend - tstart) << "\n";

       coprthr_dread(dd, dev_debug, 0, host_debug, DEBUG_BUFFER*sizeof(int),COPRTHR_E_WAIT);
//        printf("Read return data from shared memory\n");

      /// Uncomment to use debug as output
      i = 0;
        while (i<DEBUG_BUFFER)
        {
            if (host_debug[i] != -1)
//            if (i < (16*5))
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
        printf("\neod\n");
//        fout << std::endl;
//        fout.flush();
*/

/// multicast
/*
/// Uncomment if using debug

       tstart = clock();
	   coprthr_mpiexec(dd, ECORES, krnMulti, &args, sizeof(args), 0);
       tend = clock();
//       cout <<
        printf("forked - Multicast. Exec time was: %i\n", (int)(tend - tstart));

//       fout << n << "," << "multicast," << (tend - tstart) << endl;
       coprthr_dread(dd, dev_debug, 0, host_debug, DEBUG_BUFFER*sizeof(int),COPRTHR_E_WAIT);
*/
    /// Uncomment to use debug as output
 /*       i = 0;
        while (i<DEBUG_BUFFER)
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
//        for (i=0; i<DEBUG_BUFFER; i++)
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
        while (i<DEBUG_BUFFER)
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
       printf("about to call broadcast.\n");
       tstart = clock();
//       coprthr_dexec(dd, ECORES, thr_passUni, &args, COPRTHR_E_WAIT);   // wait for the documentaiton for this call
        coprthr_mpiexec(dd, ECORES, thr_Broadcast, &args, sizeof(pass_args), 0);
       tend = clock();
       printf("forked - Broadcast - no wait. Exec time was: %i\n", (int)(tend - tstart));

//       fout << n << "," << "unicast," << (tend - tstart) << endl;
        coprthr_dread(dd, dev_debug, 0, host_debug, DEBUG_BUFFER*sizeof(int), COPRTHR_E_NOWAIT);

    /// Uncomment to use debug as output
    /// First Retrieve the debug output from the cores - TODO
      i = 0;
        while (i<DEBUG_BUFFER)
        {
            if (host_debug[i] != -1)
//            if (i < (16*5))
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

//         fout.flush();
    }
    printf("closing device\n");
	coprthr_dclose(dd);
//    fbuf.close();
    return 0;
}
