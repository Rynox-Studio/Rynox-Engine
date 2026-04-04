#pragma once

#include <memory>
#include <string_view>

#include "Model.h"

namespace Rynox
{
	template<typename T>
	class ResourceLoader
	{
	public:
		static std::unique_ptr<T> Load(std::string_view path)
		{
			static_assert(sizeof(T) == 0, "ResourceLoader not implemented for this type");
            return nullptr;
		}
	};

	template<>
    std::unique_ptr<Model> ResourceLoader<Model>::Load(std::string_view path);
}
