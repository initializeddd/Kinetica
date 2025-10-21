#ifndef KINETICA_COMPONENTS_MATERIAL_HPP
#define KINETICA_COMPONENTS_MATERIAL_HPP

#include <string>
#include <glm/glm.hpp>

namespace Kinetica::Components {

    struct SMaterial {
        glm::vec3 baseColor = glm::vec3(0.8f);
        float metallic = 0.0f;
        float roughness = 0.5f;
        bool useVertexColor = false;
        std::string name = "Default";
    };

} // namespace Kinetica::Components

#endif
