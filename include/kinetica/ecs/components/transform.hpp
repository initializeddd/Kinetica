#ifndef KINETICA_COMPONENTS_TRANSFORM_HPP
#define KINETICA_COMPONENTS_TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Kinetica::Components {

    struct STransform {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        mutable bool isDirty = true;
        mutable glm::mat4 matrix = glm::mat4(1.0f);

        const glm::mat4& getMatrix() const {
            if (isDirty) {
                matrix = glm::mat4(1.0f);
                matrix = glm::translate(matrix, position);
                matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));
                matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
                matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
                matrix = glm::scale(matrix, scale);
                isDirty = false;
            }
            return matrix;
        }
    };

} // namespace Kinetica::Components

#endif
