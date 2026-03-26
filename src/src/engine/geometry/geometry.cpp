#include "engine/geometry/geometry.hpp"

#pragma warning(push, 0)
#include <glm/detail/func_geometric.inl>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <glad/glad.h>
#pragma warning(pop)

Geometry::~Geometry() {
  glDeleteBuffers(6, _VBO);
  glDeleteVertexArrays(1, &_VAO);
}

//TODO Check if works for all geometries
//http://www.opengl-tutorial.org/es/intermediate-tutorials/tutorial-13-normal-mapping/
void Geometry::calcTangents(const float* positions, const float* uvs, const float* normals,
  float* tangents, float* biTangents) const {
  if (_nVertices % 3 != 0) {
    std::cout << "Error Calculating Tangents, Vertices is not a multiple of 3" << std::endl;
    //return;
  }

  // going for triangles
  for (size_t i = 0; i < _nVertices; i += 3) {
    const uint32_t index3 = i * 3;
    const uint32_t index2 = i * 2;
    glm::vec3 v0(positions[index3 + 0], positions[index3 + 1], positions[index3 + 2]);
    glm::vec3 v1(positions[index3 + 3], positions[index3 + 4], positions[index3 + 5]);
    glm::vec3 v2(positions[index3 + 6], positions[index3 + 7], positions[index3 + 8]);

    glm::vec2 uv0(uvs[index2 + 0], uvs[index2 + 1]);
    glm::vec2 uv1(uvs[index2 + 2], uvs[index2 + 3]);
    glm::vec2 uv2(uvs[index2 + 4], uvs[index2 + 5]);

    glm::vec3 deltaPos1 = v1 - v0;
    glm::vec3 deltaPos2 = v2 - v0;

    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;

    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    glm::vec3 biTangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

    tangents[index3 + 0] = tangents[index3 + 3] = tangents[index3 + 6] = tangent.x;
    tangents[index3 + 1] = tangents[index3 + 4] = tangents[index3 + 7] = tangent.y;
    tangents[index3 + 2] = tangents[index3 + 5] = tangents[index3 + 8] = tangent.z;

    biTangents[index3 + 0] = biTangents[index3 + 3] = biTangents[index3 + 6] = biTangent.x;
    biTangents[index3 + 1] = biTangents[index3 + 4] = biTangents[index3 + 7] = biTangent.y;
    biTangents[index3 + 2] = biTangents[index3 + 5] = biTangents[index3 + 8] = biTangent.z;
  }

  // going for vertices
  for (size_t i = 0; i < _nVertices; ++i) {
    const uint32_t index3 = i * 3;

    glm::vec3 n(normals[index3 + 0], normals[index3 + 1], normals[index3 + 2]);
    glm::vec3 t(tangents[index3 + 0], tangents[index3 + 1], tangents[index3 + 2]);
    glm::vec3 b(biTangents[index3 + 0], biTangents[index3 + 1], biTangents[index3 + 2]);

    // Gram-Schmidt orthogonalize
    t = glm::normalize(t - n * glm::dot(n, t));

    // Calculate handedness
    if (glm::dot(glm::cross(n, t), b) < 0.0f) {
      t = t * -1.0f;
    }

    tangents[index3 + 0] = t.x;
    tangents[index3 + 1] = t.y;
    tangents[index3 + 2] = t.z;
  }
}

void Geometry::uploadData(const float* positions, const float* uvs, const float* normals, const uint32_t* indices) {
  const size_t length = static_cast<size_t>(_nVertices) * 3;
  const auto tangents = new float[length];
  const auto biTangents = new float[length];

  memset(biTangents, 0.0f, length * sizeof(float));
  memset(tangents, 0.0f, length * sizeof(float));

  if (_calcTangents) {
    calcTangents(positions, uvs, normals, tangents, biTangents);
  }

  glGenVertexArrays(1, &_VAO);
  glGenBuffers(6, _VBO);

  glBindVertexArray(_VAO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBO[0]);         //elements
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * _nElements, indices, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _VBO[1]);                 //positions
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _nVertices * 3, positions, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, _VBO[2]);                 //uvs
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _nVertices * 2, uvs, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, _VBO[3]);                 //normals
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _nVertices * 3, normals, GL_STATIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, _VBO[4]);                 //tangents
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _nVertices * 3, tangents, GL_STATIC_DRAW);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(3);

  glBindBuffer(GL_ARRAY_BUFFER, _VBO[5]);                 //bitangents
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _nVertices * 3, biTangents, GL_STATIC_DRAW);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(4);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  delete[] tangents;
  delete[] biTangents;
}

void Geometry::render() const {
  glBindVertexArray(_VAO);
  glDrawElements(GL_TRIANGLES, _nElements, GL_UNSIGNED_INT, 0);
}