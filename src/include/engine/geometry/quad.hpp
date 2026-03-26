#pragma once

#include "engine/geometry/geometry.hpp"

class Quad final : public Geometry {
public:
  Quad() = delete;
  explicit Quad(float size, bool calcTangents = true);

private:
  [[maybe_unused]] float _size;
};