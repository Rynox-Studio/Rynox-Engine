#pragma once

#include <cstdint>

namespace Rynox
{
    struct SubMesh
    {
        uint32_t indexOffset;
        uint32_t indexCount;
        int32_t  baseVertex;
    };
}