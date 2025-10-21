#ifndef KINETICA_WINDOW_HPP
#define KINETICA_WINDOW_HPP

#include <string>
#include <memory>
#include <functional>

struct GLFWwindow;

namespace Kinetica {

    class CWindow {
    public:
        CWindow(const std::string& title = "Kinetica", int width = 1280, int height = 720);
        ~CWindow();

        CWindow(const CWindow&) = delete;
        CWindow& operator=(const CWindow&) = delete;

        bool isValid() const { return m_bValid; }
        void show();
        void pollEvents();
        bool shouldClose() const;
        void swap();
        bool isMinimized();

        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }

        void setResizeCallback(std::function<void(int, int)> callback) {
            m_resizeCallback = std::move(callback);
        }

        std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> m_pWindow;

    private:
        void init();

        int m_width = 0;
        int m_height = 0;
        std::function<void(int, int)> m_resizeCallback;

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

        bool m_bValid = false;
    };

} // namespace Kinetica

#endif // KINETICA_WINDOW_HPP
