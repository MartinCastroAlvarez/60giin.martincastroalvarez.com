#include "engine/geometry/cube.hpp"

Cube::Cube(float size, bool calcTangents) : _size(size) {
  _calcTangents = calcTangents;
  _nVertices = 6 * 2 * 3;   //6 faces * 2 triangles * 3 vertices;
  _nElements = _nVertices;

  const float half = size / 2.0f;

  float positions[] = { -half,   -half,   half,     // Front
                          half,    -half,   half,
                          half,    half,    half,

                          -half,   -half,   half,
                          half,    half,    half,
                          -half,   half,    half,

                          half,    -half,   half,     // Right
                          half,    -half,   -half,
                          half,    half,    -half,

                          half,    -half,   half,
                          half,    half,    -half,
                          half,    half,    half,

                          half,    -half,   -half,    // Back
                          -half,   -half,   -half,
                          -half,   half,    -half,

                          half,    -half,   -half,
                          -half,   half,    -half,
                          half,    half,    -half,

                          -half,   -half,   -half,     // Left
                          -half,   -half,    half,
                          -half,   half,    half,

                          -half,   -half,    -half,
                          -half,   half,    half,
                          -half,   half,   -half,

                          -half,   -half,   -half,     // Bottom
                          half,   -half,   -half,
                          half,    -half,   half,

                          -half,   -half,   -half,
                          half,    -half,   half,
                          -half,    -half,   half,

                          -half,   half,    half,     // Top
                          half,    half,    half,
                          half,    half,    -half,

                          -half,   half,    half,
                          half,    half,    -half,
                          -half,   half,    -half };

  float uvs[] = { 0.0f, 0.0f,  // Front
                  1.0f, 0.0f,
                  1.0f, 1.0f,

                  0.0f, 0.0f,
                  1.0f, 1.0f,
                  0.0f, 1.0f,

                  0.0f, 0.0f,  // Right
                  1.0f, 0.0f,
                  1.0f, 1.0f,

                  0.0f, 0.0f,
                  1.0f, 1.0f,
                  0.0f, 1.0f,

                  0.0f, 0.0f,  // Back
                  1.0f, 0.0f,
                  1.0f, 1.0f,

                  0.0f, 0.0f,
                  1.0f, 1.0f,
                  0.0f, 1.0f,

                  0.0f, 0.0f,  // Left
                  1.0f, 0.0f,
                  1.0f, 1.0f,

                  0.0f, 0.0f,
                  1.0f, 1.0f,
                  0.0f, 1.0f,

                  0.0f, 0.0f,  // Bottom
                  1.0f, 0.0f,
                  1.0f, 1.0f,

                  0.0f, 0.0f,
                  1.0f, 1.0f,
                  0.0f, 1.0f,

                  0.0f, 0.0f,  // Top
                  1.0f, 0.0f,
                  1.0f, 1.0f,

                  0.0f, 0.0f,
                  1.0f, 1.0f,
                  0.0f, 1.0f };

  float normals[] = { 0.0f, 0.0f, 1.0f,   // Front
                      0.0f, 0.0f, 1.0f,
                      0.0f, 0.0f, 1.0f,

                      0.0f, 0.0f, 1.0f,
                      0.0f, 0.0f, 1.0f,
                      0.0f, 0.0f, 1.0f,

                      1.0f, 0.0f, 0.0f,   // Right
                      1.0f, 0.0f, 0.0f,
                      1.0f, 0.0f, 0.0f,

                      1.0f, 0.0f, 0.0f,
                      1.0f, 0.0f, 0.0f,
                      1.0f, 0.0f, 0.0f,

                      0.0f, 0.0f, -1.0f,  // Back
                      0.0f, 0.0f, -1.0f,
                      0.0f, 0.0f, -1.0f,

                      0.0f, 0.0f, -1.0f,
                      0.0f, 0.0f, -1.0f,
                      0.0f, 0.0f, -1.0f,

                      -1.0f, 0.0f, 0.0f,  // Left
                      -1.0f, 0.0f, 0.0f,
                      -1.0f, 0.0f, 0.0f,

                      -1.0f, 0.0f, 0.0f,
                      -1.0f, 0.0f, 0.0f,
                      -1.0f, 0.0f, 0.0f,

                      0.0f, -1.0f, 0.0f,  // Bottom
                      0.0f, -1.0f, 0.0f,
                      0.0f, -1.0f, 0.0f,

                      0.0f, -1.0f, 0.0f,
                      0.0f, -1.0f, 0.0f,
                      0.0f, -1.0f, 0.0f,

                      0.0f, 1.0f, 0.0f,   // Top
                      0.0f, 1.0f, 0.0f,
                      0.0f, 1.0f, 0.0f,

                      0.0f, 1.0f, 0.0f,
                      0.0f, 1.0f, 0.0f,
                      0.0f, 1.0f, 0.0f
  };

  uint32_t indices[] = {
      0, 1, 2,    3, 4, 5,        //Front
      6, 7, 8,    9, 10, 11,      //Right
      12, 13, 14, 15, 16, 17,     //Back
      18, 19, 20, 21, 22, 23,     //Left
      24, 25, 26, 27, 28, 29,     //Bottom
      30, 31, 32, 33, 34, 35 };   //Top

  uploadData(positions, uvs, normals, indices);
}