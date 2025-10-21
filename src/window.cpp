#include <kinetica/window.hpp>
#include <GLFW/glfw3.h>
#include <kinetica/log.hpp>

// Platform-specific native includes
#if defined(_WIN32)
#include <GLFW/glfw3native.h> // for glfwGetWin32Window
#elif defined(__APPLE__)
#include <GLFW/glfw3native.h> // for glfwGetCocoaWindow
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h> // for glfwGetX11Window
#endif

namespace Kinetica {
    static CWindow* get_window_instance(GLFWwindow* window) {
        return static_cast<CWindow*>(glfwGetWindowUserPointer(window));
    }

    void CWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto* self = get_window_instance(window);
        if (self) {
            self->m_width = width;
            self->m_height = height;
            if (self->m_resizeCallback) {
                self->m_resizeCallback(width, height);
            }
        }
    }

    static void glfw_error_callback(int error, const char* description) {
        KLOG_ERROR("GLFW error " + std::to_string(error) + ": " + std::string(description));
    }

    CWindow::CWindow(const std::string& title, int width, int height)
    : m_pWindow(nullptr, nullptr)  // Initialize with null + null deleter
    {
        if (!glfwInit()) {
            KLOG_ERROR("Failed to initialize GLFW");
            return;
        }

        glfwSetErrorCallback(glfw_error_callback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        GLFWwindow* raw = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!raw) {
            KLOG_ERROR("Failed to create GLFW window");
            glfwTerminate();
            return;
        }

        // Define deleter lambda
        auto deleter = [](GLFWwindow* w) {
            if (w) {
                glfwDestroyWindow(w);
                glfwTerminate();
            }
        };

        m_pWindow = std::unique_ptr<GLFWwindow, decltype(deleter)>(raw, deleter);
        m_bValid = true;

        glfwSetWindowUserPointer(raw, this);
        glfwSetFramebufferSizeCallback(raw, framebufferResizeCallback);

        glfwMakeContextCurrent(m_pWindow.get());
    }

    CWindow::~CWindow() = default;

    void CWindow::pollEvents() {
        if (m_bValid) {
            glfwPollEvents();
        }
    }

    void CWindow::swap() {
        if (m_bValid) {
            glfwSwapBuffers(m_pWindow.get());
        }
    }

    bool CWindow::isMinimized() {
        return glfwGetWindowAttrib(m_pWindow.get(), GLFW_ICONIFIED);
    }

    bool CWindow::shouldClose() const {
        return m_bValid && m_pWindow ? glfwWindowShouldClose(m_pWindow.get()) : true;
    }

} // namespace Kinetica
