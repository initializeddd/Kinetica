#include <kinetica/rendering.hpp>
#include <kinetica/window.hpp>
#include <kinetica/log.hpp>

#include <GLFW/glfw3.h>

#include <iostream>

namespace Kinetica {

    CRenderer::CRenderer(const Kinetica::CWindow& window) {

        glfwMakeContextCurrent(window.m_pWindow.get());

        if (!glGetString(GL_VERSION)) {
            KLOG_ERROR("No OpenGL context active!");
            m_bValid = false;
            return;
        }
        std::cout << "OpenGL version: " << std::string((const char*)glGetString(GL_VERSION)) << "\n";

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        m_bValid = true;
    }

    CRenderer::~CRenderer() {
        // Nothing to destroy — OpenGL state is tied to context
    }

    void CRenderer::clear() {
        if (!m_bValid) return;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void CRenderer::present() {
        // This method is a placeholder for future renderer-side post-processing.
    }

} // namespace Kinetica
