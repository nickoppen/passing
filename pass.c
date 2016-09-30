#include <stdio.h>

#include <coprthr.h>
#include <coprthr_cc.h>
#include <coprthr_thread.h>
#include <coprthr_mpi.h>

#include "passing.h"

#define ECORES 16    /// Not great - CORECOUNT is defined as the same thing in ringTopo16.c

int main()
{
    int i, j;    /// Uncomment when using debug
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

    for (n=1; n<=32; n++)
    {
        args.n = n;     /// passed to all kernels

        tstart = clock();
        coprthr_mpiexec(dd, ECORES, thr_Broadcast, &args, sizeof(pass_args), 0);
        tend = clock();
        coprthr_dread(dd, dev_debug, 0, host_debug, DEBUG_BUFFER*sizeof(int), COPRTHR_E_WAIT);

        printf("%d", n);
        for (i=0; i<4; i++)
        {
            summation = 0;
            for (j=0; j < 16; j++)
            {
                summation += host_debug[j+(i*16)];
            }
            ave = (float)summation / 16;
            printf(", %f", ave);
        }
        printf("\n");

        printf("%d", n);
        for (i=0; i<4; i++)
        {
            summation = 0;
            for (j=0; j < 16; j++)
            {
                summation += host_debug[64+j+(i*16)];
            }
            ave = (float)summation / 16;
            printf(", %f", ave);
        }
        printf("\n");

    }
	coprthr_dclose(dd);
    return 0;
}
