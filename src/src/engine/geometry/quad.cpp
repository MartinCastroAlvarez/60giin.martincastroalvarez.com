#include "engine/geometry/quad.hpp"

Quad::Quad(float size, bool calcTangents) : _size(size) {
  _calcTangents = calcTangents;
  _nVertices = 1 * 2 * 3;  //1 face * 2 triangles * 3 vertices
  _nElements = _nVertices;

  const float half = size / 2.0f;

  float positions[] = { half, half, 0.0f,    //upper right triangle
                        half, -half, 0.0f,
                        -half, half, 0.0f,

                        half, -half, 0.0f,   //lower left triangle
                        -half, half, 0.0f,
                        -half, -half, 0.0f };

  float uvs[] = { 1.0f, 1.0f,
                  1.0f, 0.0f,
                  0.0f, 1.0f,

                  1.0f, 0.0f,
                  0.0f, 1.0f,
                  0.0f, 0.0f };

  float normals[] = { 0.0f, 0.0f, 1.0f,
                      0.0f, 0.0f, 1.0f,
                      0.0f, 0.0f, 1.0f,

                      0.0f, 0.0f, 1.0f,
                      0.0f, 0.0f, 1.0f,
                      0.0f, 0.0f, 1.0f };

  uint32_t indices[] = { 0, 2, 1,
                     3 , 4, 5 };

  uploadData(positions, uvs, normals, indices);
}