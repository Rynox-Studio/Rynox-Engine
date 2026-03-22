#pragma once

#include <Common/UUID.h>
#include <Common/Ref.h>

namespace Rynox::Core
{
	using AssetHandle = Common::UUID;

	enum class AssetType
	{
		None = 0,
		Mesh,
	};

	class Asset : public Common::RefCounted
	{
	public:
		virtual ~Asset() = default;

		[[nodiscard]] AssetHandle GetHandle() const { return m_Handle; }
		[[nodiscard]] virtual AssetType GetType() const { return GetStaticType(); }
		static AssetType GetStaticType() { return AssetType::None; }

	private:
		AssetHandle m_Handle;
	};

	template<typename T>
	concept AssetConcept = std::derived_from<T, Asset> && std::derived_from<T, Common::RefCounted>;
}
