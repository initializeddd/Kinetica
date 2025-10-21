#ifndef KINETICA_COMPONENTS_TRANSFORM_HPP
#define KINETICA_COMPONENTS_TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Kinetica::Components {

    struct STransform {
        glm::vec3 position = glm::vec3(0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // wxyz
        glm::vec3 scale = glm::vec3(1.0f);
    };

} // namespace Kinetica::Components

#endif
