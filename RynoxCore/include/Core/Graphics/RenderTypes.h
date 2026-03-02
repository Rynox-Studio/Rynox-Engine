#pragma once

#include "RenderTags.h"
#include "Core/Handles.h"

namespace Rynox::Graphics
{
    using VertexBufferHandle = Rynox::GPUHandle<VertexBufferTag>;
    using IndexBufferHandle = Rynox::GPUHandle<IndexBufferTag>;
    using VertexArrayHandle = Rynox::GPUHandle<VertexArrayTag>;
    using ShaderHandle = Rynox::GPUHandle<ShaderTag>;
    using MeshHandle = Rynox::GPUHandle<MeshTag>;
}