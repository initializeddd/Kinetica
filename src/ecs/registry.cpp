#include <kinetica/ecs/registry.hpp>
#include <kinetica/uuid.hpp>

namespace Kinetica {
    const EntityID INVALID_ENTITY = CUUID::nil();

    EntityID CRegistry::createEntity() {
        EntityID id =  CUUID::generate();
        m_entities.insert(id);
        return id;
    }

    void CRegistry::destroyEntity(EntityID id) {
        if (m_entities.erase(id) == 0) return;

        for (auto& pair : m_storages) {
            pair.second->erase(id);
        }
    }

} // namespace Kinetica
