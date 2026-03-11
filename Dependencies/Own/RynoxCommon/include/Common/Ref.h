#pragma once

#include <atomic>
#include <concepts>

namespace Rynox::Common
{
	class RefCounted
	{
	public:
		RefCounted() = default;
		virtual ~RefCounted() = default;

		void IncRef() const { ++m_RefCount; }
		void DecRef() const { --m_RefCount; }

		[[nodiscard]] uint32_t GetRefCount() const { return m_RefCount.load(); }

	private:
		mutable std::atomic<uint32_t> m_RefCount{ 0 };
	};

    template<typename T>
    concept RefCountedType = std::derived_from<T, RefCounted>;

    template<RefCountedType T>
	class Ref
	{
	public:
		Ref() = default;

		Ref(std::nullptr_t n) : m_Instance(nullptr) {}

		Ref(T* instance) : m_Instance(instance) { IncRef(); }

		template<typename T2>
		requires(std::is_base_of_v<T2, T> || std::is_base_of_v<T, T2>)
		Ref(const Ref<T2>& other)
		{
			m_Instance = static_cast<T*>(other.m_Instance);
			IncRef();
		}

		template<typename T2>
		requires(std::is_base_of_v<T2, T> || std::is_base_of_v<T, T2>)
		Ref(Ref<T2>&& other) noexcept
		{
			m_Instance = static_cast<T*>(other.m_Instance);
			other.m_Instance = nullptr;
		}

		template<typename... Args>
		[[nodiscard]] static Ref<T> Create(Args&&... args)
		{
			return Ref<T>(new T(std::forward<Args>(args)...));
		}

		~Ref() { DecRef(); }

		Ref(const Ref<T>& other) : m_Instance(other.m_Instance) { IncRef(); }
		Ref(Ref<T>&& other) noexcept : m_Instance(other.m_Instance) { other.m_Instance = nullptr; }

		Ref<T>& operator=(const Ref<T>& other)
		{
			if (this != &other)
			{
				DecRef();
				m_Instance = other.m_Instance;
				IncRef();
			}
			return *this;
		}

		Ref<T>& operator=(Ref<T>&& other) noexcept
		{
			if (this != &other)
			{
				DecRef();
				m_Instance = other.m_Instance;
				other.m_Instance = nullptr;
			}
			return *this;
		}

		template<typename T2>
		[[nodiscard]] Ref<T2> As() const
		{
			return Ref<T2>(dynamic_cast<T2*>(m_Instance));
		}

		[[nodiscard]] T* operator->() { return m_Instance; }
		[[nodiscard]] const T* operator->() const { return m_Instance; }

		[[nodiscard]] T* Get() { return m_Instance; }
		[[nodiscard]] const T* Get() const { return m_Instance; }

		[[nodiscard]] T& operator*() { return *m_Instance; }
		[[nodiscard]] const T& operator*() const { return *m_Instance; }

		[[nodiscard]] operator bool() const { return m_Instance != nullptr; }

		[[nodiscard]] bool operator==(const Ref<T>& other) const
		{
			return m_Instance == other.m_Instance;
		}

		[[nodiscard]] bool operator!=(const Ref<T>& other) const
		{
			return !(*this == other);
		}
		
		void Reset()
		{
			DecRef();
			m_Instance = nullptr;
		}

	private:
		void IncRef() const
		{
			if (m_Instance)
			{
				m_Instance->IncRef();
			}
		}

		void DecRef() const
		{
			if (m_Instance)
			{
				m_Instance->DecRef();

				if (m_Instance->GetRefCount() == 0)
				{
					delete m_Instance;
					m_Instance = nullptr;
				}
			}
		}

	private:
		template<typename T2>
		friend class Ref;

		mutable T* m_Instance = nullptr;
	};
}
