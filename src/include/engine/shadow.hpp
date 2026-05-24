#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <cstdint>

class Shadow {
public:
  static Shadow* instance();

  void init(uint32_t width = 2048, uint32_t height = 2048, float nearPlane = 1.0f, float farPlane = 25.0f);
  void bindFBO() const;
  void unbindFBO(uint32_t restoreWidth, uint32_t restoreHeight) const;

  uint32_t getDepthMap() const { return _depthMap; }
  uint32_t getFBO() const { return _fbo; }
  
  // Config properties
  void setOrthoBoxSize(float size) { _orthoBoxSize = size; }
  float getOrthoBoxSize() const { return _orthoBoxSize; }

  void setDistance(float distance) { _distance = distance; }
  float getDistance() const { return _distance; }

  void setIntensity(float intensity) { _intensity = intensity; }
  float getIntensity() const { return _intensity; }

  glm::mat4 getLightSpaceMatrix(const glm::vec3& lightDirWorld) const;

private:
  Shadow() = default;
  ~Shadow();

  uint32_t _fbo = 0;
  uint32_t _depthMap = 0;

  uint32_t _width = 2048;
  uint32_t _height = 2048;
  float _nearPlane = 1.0f;
  float _farPlane = 25.0f;

  float _orthoBoxSize = 10.0f;
  float _distance = 10.0f;
  float _intensity = 0.8f;

  static Shadow* _instance;
};
