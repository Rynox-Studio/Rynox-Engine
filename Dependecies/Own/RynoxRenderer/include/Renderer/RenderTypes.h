#pragma once

#include "RenderTags.h"
#include "Handles.h"

namespace Rynox::Renderer
{
    using VertexBufferHandle = Rynox::GPUHandle<VertexBufferTag>;
    using IndexBufferHandle = Rynox::GPUHandle<IndexBufferTag>;
    using VertexArrayHandle = Rynox::GPUHandle<VertexArrayTag>;
    using ShaderHandle = Rynox::GPUHandle<ShaderTag>;
    using MeshHandle = Rynox::GPUHandle<MeshTag>;
}