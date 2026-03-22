#include "AssetManager.h"
#pragma once

namespace Rynox::Core
{
	template<AssetConcept T>
	inline Common::Ref<T> AssetManager::Load(std::string_view path)
	{
		auto it = m_PathToHandle.find(path.data());
		if (it != m_PathToHandle.end())
		{
			auto cacheIt = m_Cache.find(it->second);
			if (cacheIt != m_Cache.end())
			{
				if (auto asset = cacheIt->second.Lock())
				{
					return asset.As<T>();
				}

				m_HandleToPath.erase(it->second);
				m_Cache.erase(cacheIt);
				m_PathToHandle.erase(it);
			}
		}

		Common::Ref<T> asset = AssetLoader<T>::Load(path);
		if (!asset) return nullptr;

		AssetHandle handle = asset->GetHandle();
		m_PathToHandle[path.data()] = handle;
		m_HandleToPath[handle] = path.data();
		m_Cache[handle] = Common::WeakRef<Asset>(asset);
		return asset;
	}

	template<AssetConcept T>
	inline Common::Ref<T> Rynox::Core::AssetManager::Get(AssetHandle handle)
	{
		auto it = m_Cache.find(handle);
		if (it == m_Cache.end()) return nullptr;

		auto asset = it->second.Lock();
		if (!asset) return nullptr;

		return asset.As<T>();
	}

	template<AssetConcept T>
	inline Common::Ref<T> AssetManager::Get(AssetHandle handle) const
	{
		auto it = m_Cache.find(handle);
		if (it == m_Cache.end()) return nullptr;

		auto asset = it->second.Lock();
		if (!asset) return nullptr;

		return asset.As<T>();
	}
}
