#include "coreId16.h"

void initRing(int * NEXT,int * PREV, int * ringIndex, int * gidOrder)
{

    unsigned int gid = coprthr_corenum();
    unsigned int coreId = LOCAL_MEM_ADDRESS_BASE();
    gidOrder[0] = 0;
    gidOrder[1] = 1;
    gidOrder[2] = 2;
    gidOrder[3] = 3;
    gidOrder[4] = 7;
    gidOrder[5] = 11;
    gidOrder[6] = 15;
    gidOrder[7] = 14;
    gidOrder[8] = 10;
    gidOrder[9] = 6;
    gidOrder[10] = 5;
    gidOrder[11] = 9;
    gidOrder[12] = 13;
    gidOrder[13] = 12;
    gidOrder[14] = 8;
    gidOrder[15] = 4;

    switch(coreId)
    {
    case core00:
        (*NEXT) = core10;
        (*PREV) = core01;
        (*ringIndex) = 0;
        break;
    case core10:
        (*NEXT) = core20;
        (*PREV) = core00;
        (*ringIndex) = 1;
        break;
    case core20:
        (*NEXT) = core30;
        (*PREV) = core10;
        (*ringIndex) = 2;
        break;
    case core30:
        (*NEXT) = core31;
        (*PREV) = core20;
        (*ringIndex) = 3;
        break;
    case core31:
        (*NEXT) = core32;
        (*PREV) = core30;
        (*ringIndex) = 4;
        break;
    case core32:
        (*NEXT) = core33;
        (*PREV) = core31;
        (*ringIndex) = 5;
        break;
    case core33:
        (*NEXT) = core23;
        (*PREV) = core32;
        (*ringIndex) = 6;
        break;
    case core23:
        (*NEXT) = core22;
        (*PREV) = core33;
        (*ringIndex) = 7;
        break;
    case core22:
        (*NEXT) = core21;
        (*PREV) = core23;
        (*ringIndex) = 8;
        break;
    case core21:
        (*NEXT) = core11;
        (*PREV) = core22;
        (*ringIndex) = 9;
        break;
    case core11:
        (*NEXT) = core12;
        (*PREV) = core21;
        (*ringIndex) = 10;
        break;
    case core12:
        (*NEXT) = core13;
        (*PREV) = core11;
        (*ringIndex) = 11;
        break;
    case core13:
        (*NEXT) = core03;
        (*PREV) = core12;
        (*ringIndex) = 12;
        break;
    case core03:
        (*NEXT) = core02;
        (*PREV) = core13;
        (*ringIndex) = 13;
        break;
    case core02:
        (*NEXT) = core01;
        (*PREV) = core03;
        (*ringIndex) = 14;
        break;
    case core01:
        (*NEXT) = core00;
        (*PREV) = core02;
        (*ringIndex) = 15;
        break;
    default: ;      // don't know what to do here
    }

}

void mpi_initRing(int rank, int * next, int * prev)
{
///
/// - this assumes that core num and rank are the same !!!
///

    switch(rank)
    {
    case 0:
        *next = 1;
        *prev = 4;
    break;
    case 1:
        *next = 2;
        *prev = 0;
    break;
    case 2:
        *next = 3;
        *prev = 1;
    break;
    case 3:
        *next = 7;
        *prev = 2;
    break;
    case 4:
        *next = 0;
        *prev = 8;
    break;
    case 5:
        *next = 9;
        *prev = 6;
    break;
    case 6:
        *next = 5;
        *prev = 10;
    break;
    case 7:
        *next = 11;
        *prev = 3;
    break;
    case 8:
        *next = 4;
        *prev = 12;
    break;
    case 9:
        *next = 13;
        *prev = 5;
    break;
    case 10:
        *next = 6;
        *prev = 14;
    break;
    case 11:
        *next = 15;
        *prev = 7;
    break;
    case 12:
        *next = 8;
        *prev = 13;
    break;
    case 13:
        *next = 12;
        *prev = 9;
    break;
    case 14:
        *next = 10;
        *prev = 15;
    break;
    case 15:
        *next = 14;
        *prev = 11;
    break;
    default:
        *next = 0;
        *prev = 0;
    break;
    }
}
