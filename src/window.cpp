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
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        GLFWwindow* raw = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!raw) {
            KLOG_ERROR("Failed to create GLFW window");
            glfwTerminate();
            return;
        }

        // Define deleter lambda (captures nothing, so it's stateless)
        auto deleter = [](GLFWwindow* w) {
            if (w) {
                glfwDestroyWindow(w);
                glfwTerminate();
            }
        };

        // Assign with reset (or construct via make_unique-style, but we do this)
        m_pWindow = std::unique_ptr<GLFWwindow, decltype(deleter)>(raw, deleter);
        m_bValid = true;
    }

    CWindow::~CWindow() = default;

    void CWindow::pollEvents() {
        if (m_bValid) {
            glfwPollEvents();
        }
    }

    bool CWindow::shouldClose() const {
        return m_bValid && m_pWindow ? glfwWindowShouldClose(m_pWindow.get()) : true;
    }

} // namespace Kinetica
