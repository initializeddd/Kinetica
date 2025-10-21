#ifndef KINETICA_WINDOW_HPP
#define KINETICA_WINDOW_HPP

#include <string>
#include <memory>

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

        std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> m_pWindow;

    private:
        void init();
        bool m_bValid = false;
    };

} // namespace Kinetica

#endif // KINETICA_WINDOW_HPP
