#ifndef Engine_NavStruc_h__
#define Engine_NavStruct_h__

namespace Engine
{
    struct NavMeshHeader {
        uint32_t numNavTag;
        std::string navTag;
        uint32_t numCells = 0;
    };

    struct NavCellBinary {
        Vector3 vertices[3];      
        int32_t neighborIndices[3];
    };
}

#endif
