#ifndef KINETICA_REGISTRY_HPP
#define KINETICA_REGISTRY_HPP

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <unordered_set>

#include "components/transform.hpp"
#include "components/mesh.hpp"
#include "components/material.hpp"
#include "../uuid.hpp"

namespace Kinetica {
    using EntityID = CUUID;
    extern const EntityID INVALID_ENTITY;

    class CRegistry {
    public:
        EntityID createEntity();

        void destroyEntity(EntityID id);

        template<typename T>
        T& addComponent(EntityID entity);

        template<typename T>
        void removeComponent(EntityID entity);

        template<typename T>
        T* getComponent(EntityID entity);

        template<typename T>
        bool hasComponent(EntityID entity) const;

        std::unordered_set<EntityID> m_entities;

        std::vector<EntityID> getAllEntities() const {
            return std::vector<EntityID>(m_entities.begin(), m_entities.end());
        }

    private:
        struct IComponentStorage {
            virtual ~IComponentStorage() = default;
            virtual void erase(EntityID id) = 0;
        };

        template<typename T>
        struct ComponentStorage : public IComponentStorage {
            std::unordered_map<EntityID, T> components;

            void erase(EntityID id) override {
                components.erase(id);
            }
        };

        std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> m_storages;
    };

    // Inline template implementations
    template<typename T>
    T& CRegistry::addComponent(EntityID entity) {
        auto typeIdx = std::type_index(typeid(T));
        auto it = m_storages.find(typeIdx);
        if (it == m_storages.end()) {
            // Create new storage for type T
            auto newStorage = std::make_unique<ComponentStorage<T>>();
            it = m_storages.emplace(typeIdx, std::move(newStorage)).first;
        }
        auto& storage = static_cast<ComponentStorage<T>&>(*it->second);
        return storage.components[entity];
    }

    template<typename T>
    void CRegistry::removeComponent(EntityID entity) {
        auto it = m_storages.find(std::type_index(typeid(T)));
        if (it != m_storages.end()) {
            auto& storage = static_cast<ComponentStorage<T>&>(*it->second);
            storage.components.erase(entity);
        }
    }

    template<typename T>
    T* CRegistry::getComponent(EntityID entity) {
        auto it = m_storages.find(std::type_index(typeid(T)));
        if (it == m_storages.end()) return nullptr;
        auto& storage = static_cast<ComponentStorage<T>&>(*it->second);
        auto compIt = storage.components.find(entity);
        return (compIt != storage.components.end()) ? &compIt->second : nullptr;
    }

    template<typename T>
    bool CRegistry::hasComponent(EntityID entity) const {
        auto it = m_storages.find(std::type_index(typeid(T)));
        if (it == m_storages.end()) return false;
        auto& storage = static_cast<const ComponentStorage<T>&>(*it->second);
        return storage.components.count(entity) > 0;
    }

} // namespace Kinetica

#endif
