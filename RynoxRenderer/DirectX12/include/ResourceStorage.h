#pragma once

#include <vector>

#include <Rendering/GPUHandle.h>

namespace Rynox::DirectX12
{
    template<typename T, typename HandleTag>
    class ResourceStorage
    {
    public:
        ResourceStorage() = default;
        ~ResourceStorage() = default;

        GPUHandle<HandleTag> Add(const T& resource)
        {
            GPUHandle<HandleTag> handle;
            if (!m_Free.empty())
            {
                Entry& entry = m_Storage[m_Free.back()];
                handle.id = m_Free.back();
                handle.gen = entry.Gen;
                entry.Resource = resource;
                m_Free.pop_back();
            }
            else
            {
                m_Storage.emplace_back(resource, 0);
                Entry& entry = m_Storage.back();
                handle.id = (uint32_t)m_Storage.size();
                handle.gen = entry.Gen;
            }
            return handle;
        }

        GPUHandle<HandleTag> Add(T&& resource)
        {
            GPUHandle<HandleTag> handle;
            if (!m_Free.empty())
            {
                Entry& entry = m_Storage[m_Free.back()];
                handle.id = m_Free.back();
                handle.gen = entry.Gen;
                entry.Resource = std::move(resource);
                m_Free.pop_back();
            }
            else
            {
                m_Storage.emplace_back(std::move(resource), 0);
                Entry& entry = m_Storage.back();
                handle.id = (uint32_t)m_Storage.size();
                handle.gen = entry.Gen;
            }
            return handle;
        }

        void Remove(GPUHandle<HandleTag> handle)
        {
            if (!Contains(handle)) return;
            
            Entry& entry = m_Storage[handle.id];
            entry.Resource = {};
            entry.Gen++;

            m_Free.push_back(handle.id);
        }

        bool Contains(GPUHandle<HandleTag> handle) const
        {
            return (handle.id < m_Storage.size() && handle.gen == m_Storage[handle.id].Gen);
        }

    private:
        struct Entry
        {
            T Resource;
            uint32_t Gen = 0;
        };

        std::vector<Entry> m_Storage;
        std::vector<uint32_t> m_Free;
    };
}
