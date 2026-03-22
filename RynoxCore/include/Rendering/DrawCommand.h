#pragma once

#include "GPUHandle.h"

#include <Math/Mat4.h>

namespace Rynox
{
    struct DrawCommand
    {
        MeshHandle mesh;
        ShaderHandle shader;
        Math::Mat4 transform;
    };
}