#pragma once

#include <Math/Vec2.h>
#include <Math/Vec3.h>

namespace Rynox
{
    struct Vertex
    {
        Math::Vec3 Position;
        Math::Vec3 Normal;
        Math::Vec3 Tangent;
        Math::Vec3 Bitangent;
        Math::Vec2 TexCoords;
    };
}