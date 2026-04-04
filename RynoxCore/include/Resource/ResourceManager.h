#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include <Resource/Resource.h>
#include <Resource/ResourceLoader.h>
#include <Resource/ResourceHandle.h>

namespace Rynox
{
	class ResourceManager
	{
	public:
		ResourceManager() = default;
		~ResourceManager() = default;

		template<ResourceConcept T>
		ResourceHandle Load(std::string_view path);

		void Unload(ResourceHandle handle);
		void UnloadAll();

		bool Contains(ResourceHandle handle) const;

		template<ResourceConcept T>
		T* Get(ResourceHandle handle);

		template<ResourceConcept T>
		T* Get(std::string_view path);

		std::string GetPath(ResourceHandle handle) const;
	private:
		struct ResourceSlot
		{
			std::unique_ptr<Resource> Resource;
			std::string Path;
			uint32_t Gen = 0;
		};

		std::vector<ResourceSlot> m_Slots;
		std::vector<uint32_t> m_FreeList;

		std::unordered_map<std::string, ResourceHandle> m_PathToHandle;
	};
}

#include "ResourceManager.inl"
