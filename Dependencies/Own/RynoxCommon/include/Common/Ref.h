#pragma once

#include <atomic>
#include <concepts>

namespace Rynox::Common
{
	class RefCounted
	{
	public:
		void IncRef() const
		{
			m_RefCount.fetch_add(1, std::memory_order_relaxed);
		}

		void DecRef() const
		{
			if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				delete this;
			}
		}

		uint32_t GetRefCount() const
		{
			return m_RefCount.load();
		}

	protected:
		virtual ~RefCounted() = default;

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

        Ref(T* instance)
            : m_Instance(instance)
        {
            Inc();
        }

        Ref(const Ref& other)
            : m_Instance(other.m_Instance)
        {
            Inc();
        }

        Ref(Ref&& other) noexcept
            : m_Instance(other.m_Instance)
        {
            other.m_Instance = nullptr;
        }

        ~Ref()
        {
            Dec();
        }

        Ref& operator=(const Ref& other)
        {
            if (this == &other)
                return *this;

            Dec();
            m_Instance = other.m_Instance;
            Inc();
            return *this;
        }

        Ref& operator=(Ref&& other) noexcept
        {
            if (this == &other)
                return *this;

            Dec();
            m_Instance = other.m_Instance;
            other.m_Instance = nullptr;
            return *this;
        }

    private:
        void Inc()
        {
            if (m_Instance)
                m_Instance->IncRef();
        }

        void Dec()
        {
            if (m_Instance)
                m_Instance->DecRef();
        }

    private:
        T* m_Instance = nullptr;
    };
}
