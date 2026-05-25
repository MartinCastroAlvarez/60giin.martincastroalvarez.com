#include "engine/shadow.hpp"
#include <iostream>
#include <cmath>

Shadow* Shadow::_instance = nullptr;

Shadow* Shadow::instance() {
  if (_instance == nullptr) {
    _instance = new Shadow();
  }
  return _instance;
}

Shadow::~Shadow() {
  if (_fbo != 0) {
    glDeleteFramebuffers(1, &_fbo);
  }
  if (_depthMap != 0) {
    glDeleteTextures(1, &_depthMap);
  }
}

void Shadow::init(uint32_t width, uint32_t height, float nearPlane, float farPlane) {
  _width = width;
  _height = height;
  _nearPlane = nearPlane;
  _farPlane = farPlane;

  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  
  glGenTextures(1, &_depthMap);
  glBindTexture(GL_TEXTURE_2D, _depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  
  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadow::bindFBO() const {
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glViewport(0, 0, _width, _height);
}

void Shadow::unbindFBO(uint32_t restoreWidth, uint32_t restoreHeight) const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, restoreWidth, restoreHeight);
}

glm::mat4 Shadow::getLightSpaceMatrix(const glm::vec3& lightDirWorld) const {
  glm::vec3 lightPosForShadow = -lightDirWorld * _distance;
  glm::mat4 lightProjection = glm::ortho(-_orthoBoxSize, _orthoBoxSize, -_orthoBoxSize, _orthoBoxSize, _nearPlane, _farPlane);
  
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  if (std::abs(glm::dot(glm::normalize(lightPosForShadow), up)) > 0.999f) {
    up = glm::vec3(0.0f, 0.0f, 1.0f);
  }
  
  glm::mat4 lightView = glm::lookAt(lightPosForShadow, glm::vec3(0.0f), up);
  return lightProjection * lightView;
}
