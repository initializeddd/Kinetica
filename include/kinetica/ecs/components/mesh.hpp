#ifndef KINETICA_COMPONENTS_MESH_HPP
#define KINETICA_COMPONENTS_MESH_HPP

#include <vector>
#include <cstdint>
#include <GL/glew.h>

namespace Kinetica::Components {

    struct SVertex {
        float x, y, z;    // position
        float nx, ny, nz; // normal
        float u, v;       // uv
    };

    struct SIndex {
        std::uint32_t a, b, c;
    };

    struct SMesh {
        std::vector<SVertex> vertices;
        std::vector<SIndex> indices;
        bool isDirty = true;

        // --- GPU resources ---
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;

        // Helper
        GLsizei vertexCount() const { return static_cast<GLsizei>(vertices.size()); }
        GLsizei indexCount() const { return static_cast<GLsizei>(indices.size() * 3); }
    };

} // namespace Kinetica::Components

#endif
