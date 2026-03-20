#pragma once

#include "GPUHandle.h"

#include <Math/Mat4.h>

namespace Rynox::Graphics
{
    struct DrawCommand
    {
        Graphics::MeshHandle mesh;
        Graphics::ShaderHandle shader;
        Math::Mat4 transform;
    };
}