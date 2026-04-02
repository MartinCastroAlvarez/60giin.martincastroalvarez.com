#pragma once

#include "engine/geometry/geometry.hpp"

class Cube final : public Geometry {
public:
  Cube() = delete;
  explicit Cube(float size, bool calcTangents = true);

private:
  [[maybe_unused]] float _size;
};