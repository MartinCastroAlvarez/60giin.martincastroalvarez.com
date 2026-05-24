#pragma once
#include "engine/geometry/geometry.hpp"

class Pyramid final : public Geometry {
public:
  Pyramid() = delete;
  explicit Pyramid(float height = 1.0f, float baseWidth = 1.0f, bool calcTangents = true);

private:
  [[maybe_unused]] float _height;
  [[maybe_unused]] float _baseWidth;
};
