#include "coreId16.inc"

void initRing(unsigned int * RIGHT,unsigned int * LEFT, unsigned int * ringOrder)
{

    unsigned int gid = get_global_id(0);
    unsigned int coreId = LOCAL_MEM_ADDRESS_BASE(gid);

    switch(coreId)
    {
    case core00:
        (*RIGHT) = core10;
        (*LEFT) = 0x0;
        (*ringOrder) = 0;
        break;
    case core10:
        (*RIGHT) = core20;
        (*LEFT) = core00;
        (*ringOrder) = 1;
        break;
    case core20:
        (*RIGHT) = core30;
        (*LEFT) = core10;
        (*ringOrder) = 2;
        break;
    case core30:
        (*RIGHT) = core31;
        (*LEFT) = core20;
        (*ringOrder) = 3;
        break;
    case core31:
        (*RIGHT) = core21;
        (*LEFT) = core30;
        (*ringOrder) = 4;
        break;
    case core21:
        (*RIGHT) = core11;
        (*LEFT) = core31;
        (*ringOrder) = 5;
        break;
    case core11:
        (*RIGHT) = core01;
        (*LEFT) = core21;
        (*ringOrder) = 6;
        break;
    case core01:
        (*RIGHT) = core02;
        (*LEFT) = core11;
        (*ringOrder) = 7;
        break;
    case core02:
        (*RIGHT) = core12;
        (*LEFT) = core01;
        (*ringOrder) = 8;
        break;
    case core12:
        (*RIGHT) = core22;
        (*LEFT) = core02;
        (*ringOrder) = 9;
        break;
    case core22:
        (*RIGHT) = core32;
        (*LEFT) = core12;
        (*ringOrder) = 10;
        break;
    case core32:
        (*RIGHT) = core33;
        (*LEFT) = core22;
        (*ringOrder) = 11;
        break;
    case core33:
        (*RIGHT) = core23;
        (*LEFT) = core32;
        (*ringOrder) = 12;
        break;
    case core23:
        (*RIGHT) = core13;
        (*LEFT) = core33;
        (*ringOrder) = 13;
        break;
    case core13:
        (*RIGHT) = core03;
        (*LEFT) = core23;
        (*ringOrder) = 14;
        break;
    case core03:
        (*RIGHT) = 0x0;
        (*LEFT) = core13;
        (*ringOrder) = 15;
        break;
    default: ;      // don't know what to do here
    }

}
