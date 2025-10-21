#ifndef KINETICA_COMPONENTS_MESH_HPP
#define KINETICA_COMPONENTS_MESH_HPP

#include <vector>
#include <cstdint>

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
    };

} // namespace Kinetica::Components

#endif
