#pragma once

#include <string>
#include "Resource.h"

namespace Rynox 
{
    struct Shader : public Resource
    {
        std::string VertexSource;
        std::string FragmentSource;
    };
}