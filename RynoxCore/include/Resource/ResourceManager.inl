#pragma once
#include "ResourceManager.h"

namespace Rynox
{
	template<ResourceConcept T>
	ResourceHandle ResourceManager::Load(std::string_view path)
	{
		auto it = m_PathToHandle.find(path.data());
		if (it != m_PathToHandle.end())
		{
			return it->second;
		}

		std::unique_ptr<T> asset = ResourceLoader<T>::Load(path);
		if (!asset) return {};

		ResourceHandle handle;
		if (!m_FreeList.empty())
		{
			handle.Index = m_FreeList.back();
			m_FreeList.pop_back();
			handle.Gen = m_Slots[handle.Index].Gen;
			m_Slots[handle.Index] = {std::move(asset), path.data(), handle.Gen};
		}
		else
		{
			handle.Index = static_cast<uint32_t>(m_Slots.size());
			handle.Gen = 0;
			m_Slots.emplace_back(std::move(asset), path.data(), 0);
		}
		m_PathToHandle[path.data()] = handle;
		return handle;
	}

	void ResourceManager::Unload(ResourceHandle handle)
	{
		if (handle.Index >= m_Slots.size()) return;
		auto& slot = m_Slots[handle.Index];
		if (slot.Gen != handle.Gen) return;

		slot.Resource.reset();
		slot.Gen++;
		m_FreeList.push_back(handle.Index);

		auto pathIt = m_PathToHandle.find(slot.Path);
		if(pathIt != m_PathToHandle.end())
		{
			m_PathToHandle.erase(pathIt);
		}
		slot.Path.clear();
	}
	void ResourceManager::UnloadAll()
	{
		for (auto& slot : m_Slots)
		{
			slot.Resource.reset();
			slot.Path.clear();
			slot.Gen++;
		}
		m_FreeList.clear();
		m_FreeList.reserve(m_Slots.size());
		for (uint32_t i = 0; i < m_Slots.size(); i++)
		{
			m_FreeList.push_back(i);
		}
		m_PathToHandle.clear();
	}

	bool ResourceManager::Contains(ResourceHandle handle) const
	{
		if (handle.Index >= m_Slots.size()) return false;
		const auto& slot = m_Slots[handle.Index];
		return slot.Gen == handle.Gen;
	}

	template<ResourceConcept T>
	T* ResourceManager::Get(ResourceHandle handle)
	{
		if(handle.Index >= m_Slots.size()) return nullptr;
		auto& slot = m_Slots[handle.Index];
		if (slot.Gen != handle.Gen) return nullptr;
		return static_cast<T*>(slot.Resource.get());
	}

	template<ResourceConcept T>
	T* ResourceManager::Get(std::string_view path)
	{
		auto it = m_PathToHandle.find(path.data());
		if (it == m_PathToHandle.end()) return nullptr;
		return Get<T>(it->second);
	}

	std::string ResourceManager::GetPath(ResourceHandle handle) const
	{
		if(handle.Index >= m_Slots.size()) return {};
		const auto& slot = m_Slots[handle.Index];
		if (slot.Gen != handle.Gen) return {};
		return slot.Path;
	}
}
