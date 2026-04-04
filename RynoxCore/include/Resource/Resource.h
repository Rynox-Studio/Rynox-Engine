#pragma once

#include <concepts>

namespace Rynox
{
	enum class ResourceType
	{
		None = 0,
		Mesh,
		Model,
		Shader
	};

	struct Resource
	{
		static ResourceType GetStaticType() { return ResourceType::None; }
		[[nodiscard]] virtual ResourceType GetType() const = 0;
	};

	template<typename T>
	concept ResourceConcept = std::derived_from<T, Resource>;
}
