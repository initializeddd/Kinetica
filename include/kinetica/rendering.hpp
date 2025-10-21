#ifndef KINETICA_RENDERING_HPP
#define KINETICA_RENDERING_HPP

#ifdef KINETICA_WINDOW_HPP
#error "Include rendering.hpp before window.hpp"
#endif

#define GLEW_EXPERIMENTAL

#include <GL/glew.h>
#include <string>

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
        void present(); // alias for swap (to be called after render)

    private:
        bool m_bValid = false;
    };

} // namespace Kinetica

#endif // KINETICA_RENDERING_HPP
