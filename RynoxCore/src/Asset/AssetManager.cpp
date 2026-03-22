#include <Asset/AssetManager.h>

namespace Rynox::Core
{
	void AssetManager::Unload(AssetHandle handle)
	{
		auto it = m_Cache.find(handle);
		if (it == m_Cache.end()) return;

		auto pathIt = m_HandleToPath.find(handle);
		if (pathIt != m_HandleToPath.end())
		{
			m_PathToHandle.erase(pathIt->second);
			m_HandleToPath.erase(pathIt);
		}

		m_Cache.erase(it);
	}

	void AssetManager::UnloadAll()
	{
		m_Cache.clear();
		m_PathToHandle.clear();
		m_HandleToPath.clear();
	}

	bool AssetManager::Contains(AssetHandle handle) const
	{
		auto it = m_Cache.find(handle);
		return (it != m_Cache.end() && it->second.IsValid());
	}
}
