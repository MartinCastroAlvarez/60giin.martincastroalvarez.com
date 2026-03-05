#pragma once

#include <cstdint>

class GLFWwindow;

class Window {
    public:
        static Window* instance();

        [[nodiscard]]  bool isAlive()const ;
        void frame() const;

        [[nodiscard]] GLFWwindow* getNativeWindow() const {
            return _window;
        }

        void setWidth(uint32_t width) { _width = width; }
        void setHeight(uint32_t height) { _height = height; }

        [[nodiscard]] uint32_t getWidth() const { return _width; }
        [[nodiscard]] uint32_t getHeight() const { return _height; }

        static bool isKeyPressed(int key);

        void setCaptureMouse(bool toggle) const;

    private:
        Window();
        ~Window();
        Window(const Window&) = default;
        Window(Window&&) = default;
        Window& operator=(const Window&) = default;
        Window& operator=(Window&&) = default;

        GLFWwindow* _window;
        uint32_t _width = 800;
        uint32_t _height = 800;
};