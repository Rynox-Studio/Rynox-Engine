#pragma once

#include "GPUHandle.h"
#include "SubMesh.h"
#include <Math/Mat4.h>

namespace Rynox
{
    struct DrawCommand
    {
        GeometryHandle Geometry;
        SubMesh SubMesh;
        ShaderHandle Shader;
        Math::Mat4 Transform;
    };
}