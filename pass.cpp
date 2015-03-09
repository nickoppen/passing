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

   cl_kernel krn = clsym(stdacc, openHandle, "k_pass", CLLD_NOW);

   clGetKernelInfo(krn, CL_KERNEL_FUNCTION_NAME, sizeof(strInfo), strInfo, NULL);
   cout << "Got kernel called: " << strInfo << "\n";
   //fprintf(pFile, "The kernel is called: %s\n", strInfo);

    for (n=3; n<=4; n++)
    {

       clndrange_t ndr = clndrange_init1d(0, 16, 16);

        for (i=0; i<1024; i++)
            debug[i] = -1;
       clmsync(stdacc, 0, debug, CL_MEM_DEVICE|CL_EVENT_WAIT);

       tstart = clock();
       clforka(stdacc, 0, krn, &ndr, CL_EVENT_WAIT, n, 1, debug);
       tend = clock();
//       fout << "unipass," << n << "," << tstart << "," << tend << "," << (tend - tstart) << endl;
       fout << "unipass," << n << ","  << (tend - tstart) << endl;
       clmsync(stdacc, 0, debug, CL_MEM_HOST|CL_EVENT_WAIT);
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
        fout << endl;
        fout.flush();

break;

        for (i=0; i<1024; i++)
            debug[i] = -1;
       clmsync(stdacc, 0, debug, CL_MEM_DEVICE|CL_EVENT_WAIT);

       tstart = clock();
       clforka(stdacc, 0, krn, &ndr, CL_EVENT_WAIT, n, 2, debug);
       tend = clock();
//       fout << "multipass," << n << "," << tstart << "," << tend << "," << (tend - tstart) << endl;
       fout << "multipass," << n << "," << (tend - tstart) << endl;
       clmsync(stdacc, 0, debug, CL_MEM_HOST|CL_EVENT_WAIT);
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
        fout << endl;
        fout.flush();


        for (i=0; i<1024; i++)
            debug[i] = -1;
       clmsync(stdacc, 0, debug, CL_MEM_DEVICE|CL_EVENT_WAIT);

       tstart = clock();
       clforka(stdacc, 0, krn, &ndr, CL_EVENT_WAIT, n, 0, debug);
       tend = clock();
//       fout << "broadcast," << n << "," << tstart << "," << tend << "," << (tend - tstart) << endl;
       fout << "broadcast," << n << "," << (tend - tstart) << endl;
       clmsync(stdacc, 0, debug, CL_MEM_HOST|CL_EVENT_WAIT);
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
        fout << endl;
        fout.flush();
    }

/*    i = 0;
    while (i<256)
    {
        cout << debug[i++];
        if ((i%16) == 0)
            cout << endl;
        else
            cout << ",\t";
    }*/
//        cout << std::dec <<  i << "," << std::hex << cid[i] << endl;
    fbuf.close();
    return 0;
}
