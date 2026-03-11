#pragma once

#include <Common/SparseContainer.h>
#include <ECS/Entity.h>

namespace Rynox::ECS
{
    class IComponentStorage
    {
    public:
        virtual ~IComponentStorage() = default;

        virtual bool Remove(EntityID id) = 0;
    };

    template<typename T>
    class ComponentStorage : public IComponentStorage
    {
    public:
        using size_type = uint32_t;
        using value_type = T;

        [[nodiscard]] value_type& Add(EntityID id, const value_type& component) { return m_Store.Insert(id, component); }
        [[nodiscard]] value_type& Add(EntityID id, value_type&& component) { return m_Store.Insert(id, std::move(component)); }

        template<typename... Args>
        [[nodiscard]] value_type& Emplace(EntityID id, Args&&... args) { return m_Store.Emplace(id, std::forward<Args>(args)...); }

        [[nodiscard]] bool Remove(EntityID id) override { return m_Store.Remove(id); }
        [[nodiscard]] bool Contains(EntityID id) const { return m_Store.Contains(id); }

        [[nodiscard]] value_type* Get(EntityID id) { return m_Store.Get(id); }
        [[nodiscard]] const value_type* Get(EntityID id) const { return m_Store.Get(id); }

        [[nodiscard]] std::span<value_type> Elements() { return m_Store.Elements(); }
        [[nodiscard]] std::span<const value_type> Elements() const { return m_Store.Elements(); }
        [[nodiscard]] std::span<const EntityID> Entities() const { return m_Store.IDs(); }

        [[nodiscard]] size_type Size() const { return m_Store.Size(); }

    private:
        Common::SparseContainer<value_type, EntityID, size_type> m_Store;
    };
}
