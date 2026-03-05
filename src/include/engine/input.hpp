#pragma once

#include "engine/window.hpp"

#include <functional>

class Input {
    public:
        static Input* instance() {
            static Input i;
            return &i;
        }

        static bool isKeyPressed(int key) {
            return Window::instance()->isKeyPressed(key);
        }

        void setKeyPressedCallback(const std::function<void(int, int)>& f) {
            _keyPressedCallback = f;
        }

        void setMouseMoveCallback(const std::function<void(float, float)>& f) {
            _mouseMoveCallback = f;
        }

        void setScrollMoveCallback(const std::function<void(float, float)>& f) {
            _scrollMoveCallback = f;
        }

        void keyPressed(int key, int action) const {
            if (_keyPressedCallback) {
                _keyPressedCallback(key, action);
            }
        }

        void mouseMoved(float xpos, float ypos) const {
            if (_mouseMoveCallback) {
                _mouseMoveCallback(xpos, ypos);
            }
        }

        void scrollMoved(float xoffset, float yoffset) const {
            if (_scrollMoveCallback) {
                _scrollMoveCallback(xoffset, yoffset);
            }
        }

    private:
        std::function<void(int, int)> _keyPressedCallback = nullptr;
        std::function<void(float, float)> _mouseMoveCallback = nullptr;
        std::function<void(float, float)> _scrollMoveCallback = nullptr;
};