#include <iostream>
#include <fstream>
#include <stdcl.h>
#include <ctime>

using namespace std;

int main()
{
    int i, n;
    clock_t tstart, tend;
    filebuf fbuf;
    fbuf.open("pass.csv", std::ios::out);
    ostream fout(&fbuf);
    char strInfo[128];

   cl_int * debug = (cl_int*)clmalloc(stdacc, 1024 * sizeof(cl_int), 0);

   void * openHandle = clopen(stdacc, 0, CLLD_NOW);

   cl_kernel krnUni = clsym(stdacc, openHandle, "k_passUni", CLLD_NOW);
   clGetKernelInfo(krnUni, CL_KERNEL_FUNCTION_NAME, sizeof(strInfo), strInfo, NULL);
   cout << "Got kernel called: " << strInfo << "\n";

   cl_kernel krnMulti = clsym(stdacc, openHandle, "k_passMulti", CLLD_NOW);
   clGetKernelInfo(krnMulti, CL_KERNEL_FUNCTION_NAME, sizeof(strInfo), strInfo, NULL);
   cout << "Got kernel called: " << strInfo << "\n";

   cl_kernel krnBroadcast = clsym(stdacc, openHandle, "k_passBroadcastWait", CLLD_NOW);
   clGetKernelInfo(krnBroadcast, CL_KERNEL_FUNCTION_NAME, sizeof(strInfo), strInfo, NULL);
   cout << "Got kernel called: " << strInfo << "\n";

   cl_kernel krnNoWait = clsym(stdacc, openHandle, "k_passBroadcastNoWait", CLLD_NOW);
   clGetKernelInfo(krnNoWait, CL_KERNEL_FUNCTION_NAME, sizeof(strInfo), strInfo, NULL);
   cout << "Got kernel called: " << strInfo << "\n";

   clndrange_t ndr = clndrange_init1d(0, 16, 16);

    for (n=1; n<=16; n++)
    {

/// unicast
//        for (i=0; i<1024; i++)
//            debug[i] = -1;
       clmsync(stdacc, 0, debug, CL_MEM_DEVICE|CL_EVENT_WAIT);

       tstart = clock();
       clforka(stdacc, 0, krnUni, &ndr, CL_EVENT_WAIT, n, debug);
       tend = clock();

       fout << n << "," << "unicast," << (tend - tstart) << endl;
       clmsync(stdacc, 0, debug, CL_MEM_HOST|CL_EVENT_WAIT);

      /// Uncomment to use debug as output
/*      i = 0;
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
        fout << endl; */
        fout.flush();


/// multicast
//        for (i=0; i<1024; i++)
//            debug[i] = -1;
       clmsync(stdacc, 0, debug, CL_MEM_DEVICE|CL_EVENT_WAIT);

       tstart = clock();
       clforka(stdacc, 0, krnMulti, &ndr, CL_EVENT_WAIT, n, debug);
       tend = clock();

       fout << n << "," << "multicast," << (tend - tstart) << endl;
       clmsync(stdacc, 0, debug, CL_MEM_HOST|CL_EVENT_WAIT);

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
        fout << endl;   */
        fout.flush();

/// broardcast
        for (i=0; i<1024; i++)
            debug[i] = -1;
       clmsync(stdacc, 0, debug, CL_MEM_DEVICE|CL_EVENT_WAIT);

       tstart = clock();
       clforka(stdacc, 0, krnBroadcast, &ndr, CL_EVENT_WAIT, n, debug);
       tend = clock();

       fout << n << "," << "broadcast," << (tend - tstart) << endl;
       clmsync(stdacc, 0, debug, CL_MEM_HOST|CL_EVENT_WAIT);

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
   */     fout.flush();


/// broardcast - No Wait
        for (i=0; i<1024; i++)
            debug[i] = -1;
       clmsync(stdacc, 0, debug, CL_MEM_DEVICE|CL_EVENT_WAIT);

       tstart = clock();
       clforka(stdacc, 0, krnNoWait, &ndr, CL_EVENT_WAIT, n, debug);
       tend = clock();

       fout << n << "," << "broadcastNoWait," << (tend - tstart) << endl;
       clmsync(stdacc, 0, debug, CL_MEM_HOST|CL_EVENT_WAIT);

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
   */     fout.flush();
    }

    fbuf.close();
    return 0;
}
