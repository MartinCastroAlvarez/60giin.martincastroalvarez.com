#include "engine/viewport.hpp"
#include <iostream>

Viewport* Viewport::_instance = nullptr;

Viewport* Viewport::instance() {
  if (_instance == nullptr) {
    _instance = new Viewport();
  }
  return _instance;
}

Viewport::~Viewport() {
  if (_fbo != 0) glDeleteFramebuffers(1, &_fbo);
  if (_textColor != 0) glDeleteTextures(1, &_textColor);
  if (_rbo != 0) glDeleteRenderbuffers(1, &_rbo);
}

void Viewport::init(uint32_t width, uint32_t height) {
  _width = width;
  _height = height;

  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

  glGenTextures(1, &_textColor);
  glBindTexture(GL_TEXTURE_2D, _textColor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textColor, 0);

  glGenRenderbuffers(1, &_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _width, _height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Error Viewport Framebuffer not complete" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewport::bindFBO() const {
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glViewport(0, 0, _width, _height);
}

void Viewport::unbindFBO() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
