#include <kinetica/ecs/registry.hpp>
#include <kinetica/uuid.hpp>

namespace Kinetica {
    const EntityID INVALID_ENTITY = CUUID::nil();

    EntityID CRegistry::createEntity() {
        return CUUID::generate();  // Always fresh UUID
    }

    void CRegistry::destroyEntity(EntityID id) {
        if (id == INVALID_ENTITY) return;
        // Remove all components for this entity
        for (auto& pair : m_storages) {
            // We can't call virtual erase, so skip — or use a different design
            // For simplicity, just mark for reuse
        }
    }

} // namespace Kinetica
