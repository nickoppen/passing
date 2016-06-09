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
