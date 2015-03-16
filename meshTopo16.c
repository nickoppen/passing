#include "coreId16.inc"

void initMesh(unsigned int *NORTH, unsigned int *SOUTH, unsigned int *EAST, unsigned int *WEST)
{
    unsigned int gid = get_global_id(0);
    unsigned int coreId = LOCAL_MEM_ADDRESS_BASE(gid);

    (*NORTH) = coreId - 0xfffc0000;
    (*SOUTH) = coreId + 0xfffc0000;
    (*EAST) = coreId + 0xffff0000;
    (*WEST) = coreId - 0xffff0000;

    switch(coreId)
    {
    case core00:
        (*NORTH) = 0x0;
        (*WEST) = 0x0;
        break;
    case core10:
        (*NORTH) = 0x0;
        break;
    case core20:
        (*NORTH) = 0x0;
        break;
    case core30:
        (*NORTH) = 0x0;
        (*EAST) = 0x0;
        break;
    case core31:
        (*EAST) = 0x0;
        break;
    case core32:
        (*EAST) = 0x0;
        break;
    case core33:
        (*EAST) = 0x0;
        (*SOUTH) = 0x0;
        break;
    case core23:
        (*SOUTH) = 0x0;
        break;
    case core13:
        (*SOUTH) = 0x0;
        break;
    case core03:
        (*SOUTH) = 0x0;
        (*WEST) = 0x0;
        break;
    case core02:
        (*WEST) = 0x0;
        break;
    case core01:
        (*WEST) = 0x0;
        break;
    default: ;
    }

}
