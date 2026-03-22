#pragma once

#include <unordered_map>
#include <string>

#include <Asset/Asset.h>
#include <Asset/AssetLoader.h>

namespace Rynox::Core
{
	class AssetManager
	{
	public:
		AssetManager() = default;
		~AssetManager() = default;

		template<AssetConcept T>
		Common::Ref<T> Load(std::string_view path);

		void Unload(AssetHandle handle);
		void UnloadAll();

		bool Contains(AssetHandle handle) const;

		template<AssetConcept T>
		Common::Ref<T> Get(AssetHandle handle);

		template<AssetConcept T>
		Common::Ref<T> Get(AssetHandle handle) const;

	private:
		std::unordered_map<AssetHandle, Common::WeakRef<Asset>> m_Cache;
		std::unordered_map<std::string, AssetHandle> m_PathToHandle;
		std::unordered_map<AssetHandle, std::string> m_HandleToPath;
	};
}

#include "AssetManager.inl"
