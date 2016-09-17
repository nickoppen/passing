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
    int host_debug[DEBUG_BUFFER];
    pass_args args;

    float ave;
    int summation;

	int dd = coprthr_dopen(COPRTHR_DEVICE_E32,COPRTHR_O_THREAD);
    printf("epiphany dev:%i\n", dd);
	if (dd<0)
	{
        printf("Device open failed.\n");
        exit(0);
    }

	coprthr_program_t prg = coprthr_cc_read_bin("./passing.e32", 0);
	coprthr_kernel_t thr_Broadcast = (coprthr_sym_t)coprthr_getsym(prg,"k_passBroadcast");

       coprthr_mem_t dev_debug = (coprthr_mem_t)coprthr_dmalloc(dd, DEBUG_BUFFER*sizeof(int), 0);             /// Allocate some space on the epiphany for debug
        for (i=0; i<DEBUG_BUFFER; i++)
            host_debug[i] = -1;
        coprthr_dwrite(dd, dev_debug, 0, host_debug, DEBUG_BUFFER*sizeof(int), COPRTHR_E_WAIT);
        args.debug = coprthr_memptr(dev_debug, 0);

//    for (n=1; n<=16; n++)
    for (n=3; n<=3; n=n+2)
    {
        //std::cout << "n is equal to: " << n << "\n";
        args.n = n;     /// passed to all kernels


/// broardcast - No Wait

/// Uncomment if using debug
       printf("about to call broadcast.\n");
       tstart = clock();
//       coprthr_dexec(dd, ECORES, thr_passUni, &args, COPRTHR_E_WAIT);   // wait for the documentaiton for this call
        coprthr_mpiexec(dd, ECORES, thr_Broadcast, &args, sizeof(pass_args), 0);
       tend = clock();
       printf("forked - Broadcast - no wait. Exec time was: %i\n", (int)(tend - tstart));

//       fout << n << "," << "unicast," << (tend - tstart) << endl;
        coprthr_dread(dd, dev_debug, 0, host_debug, DEBUG_BUFFER*sizeof(int), COPRTHR_E_WAIT);

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
    printf("\nclosing device:%i\n", dd);
	coprthr_dclose(dd);
	for (i=0; i<3; i++)
	{
        summation = 0;
        for (n=0; n < 16; n++)
        {
            summation += host_debug[n+(i*16)];
        }
        ave = (float)summation / 16;
        printf("the average for strategy %d is %f\n", i, ave);
	}
//    fbuf.close();
    return 0;
}
