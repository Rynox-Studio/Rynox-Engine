#pragma once

#include <cstdint>

namespace Rynox::Renderer::OpenGL
{
    template<typename T, typename Data>
    struct OpenGLResourceSlot {
        T resource{};
        Data cpuData{};
        uint32_t generation = 0;
        bool active = false;
        bool queued = false;
    };
}