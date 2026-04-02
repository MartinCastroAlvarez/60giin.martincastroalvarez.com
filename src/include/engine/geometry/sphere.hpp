#pragma once

#include "engine/geometry/geometry.hpp"

#include <vector>

class Sphere final : public Geometry {
    public:
        Sphere() = delete;
        Sphere(float radius, uint32_t stacks, uint32_t slices, bool calcTangents = false);

    private:
        void generateVertexData(float* positions, float* uvs, float* normals, uint32_t* indices) const;

        float _radius;
        uint32_t _stacks, _slices;
};