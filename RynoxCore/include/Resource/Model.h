#pragma once

#include <vector>
#include "Resource.h"
#include "Rendering/Vertex.h"
#include "Rendering/SubMesh.h"

namespace Rynox 
{
    struct Model : public Resource
    {
        static ResourceType GetStaticType() { return ResourceType::Model; }
		[[nodiscard]] virtual ResourceType GetType() const override { return GetStaticType(); }

        std::vector<SubMesh> SubMeshes;
        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;
    };
}