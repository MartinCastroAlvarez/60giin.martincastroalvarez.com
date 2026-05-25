#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <cstdint>

class Viewport {
public:
  static Viewport* instance();

  void init(uint32_t width, uint32_t height);
  void bindFBO() const;
  void unbindFBO() const;

  uint32_t getTexture() const { return _textColor; }
  uint32_t getFBO() const { return _fbo; }
  
  uint32_t getWidth() const { return _width; }
  uint32_t getHeight() const { return _height; }

private:
  Viewport() = default;
  ~Viewport();

  uint32_t _fbo = 0;
  uint32_t _rbo = 0;
  uint32_t _textColor = 0;

  uint32_t _width = 300;
  uint32_t _height = 200;

  static Viewport* _instance;
};
