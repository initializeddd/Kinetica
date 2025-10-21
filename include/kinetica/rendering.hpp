#ifndef KINETICA_RENDERING_HPP
#define KINETICA_RENDERING_HPP

#ifdef KINETICA_WINDOW_HPP
#error "Include rendering.hpp before window.hpp"
#endif

#include <GL/glew.h>
#include <string>


#include <kinetica/ecs/registry.hpp>

#include <kinetica/ecs/components/transform.hpp>
#include <kinetica/ecs/components/material.hpp>
#include <kinetica/ecs/components/mesh.hpp>

#include <kinetica/window.hpp>

namespace Kinetica {

    class CRenderer {
    public:
        CRenderer(const Kinetica::CWindow& window);
        ~CRenderer();

        CRenderer(const CRenderer&) = delete;
        CRenderer& operator=(const CRenderer&) = delete;

        bool isValid() const { return m_bValid; }
        void clear();
        void present();

        void renderEntity(
            const Components::STransform& transform,
            const Components::SMesh& mesh,
            const Components::SMaterial& material
        );

        void setViewProjection(const glm::mat4& view, const glm::mat4& proj);
        void uploadMesh(Kinetica::Components::SMesh& mesh);

    private:
        bool m_bValid = false;

        GLuint m_shaderProgram = 0;

        GLint m_uModelLoc = -1;
        GLint m_uBaseColorLoc = -1;
        GLint m_uMetallicLoc = -1;
        GLint m_uRoughnessLoc = -1;
        GLint m_uUseVertexColorLoc = -1;
    };

} // namespace Kinetica

#endif // KINETICA_RENDERING_HPP
