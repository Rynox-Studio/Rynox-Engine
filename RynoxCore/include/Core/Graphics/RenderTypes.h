#pragma once

#include "RenderTags.h"
#include "Core/Handles.h"

namespace Rynox::Graphics
{
    using ShaderHandle = Rynox::GPUHandle<ShaderTag>;
    using MeshHandle = Rynox::GPUHandle<MeshTag>;
}