#include "engine/geometry/pyramid.hpp"
#include <glm/glm.hpp>
#include <cmath>

Pyramid::Pyramid(float height, float baseWidth, bool calcTangents) : _height(height), _baseWidth(baseWidth) {
  _calcTangents = calcTangents;
  _nVertices = 6 * 3; // 4 sides + 1 base (2 tris) -> 6 triangles
  _nElements = _nVertices;

  float hw = baseWidth / 2.0f;
  float h = height / 2.0f;

  // Normal for sides
  // Side front: (0,h,0), (-hw,-h,hw), (hw,-h,hw)
  // v1 = (-hw, -2h, hw), v2 = (hw, -2h, hw) - wait, from tip to base
  // Let's just use cross products for accurate normals
  glm::vec3 tip(0, h, 0);
  glm::vec3 frontLeft(-hw, -h, hw);
  glm::vec3 frontRight(hw, -h, hw);
  glm::vec3 backLeft(-hw, -h, -hw);
  glm::vec3 backRight(hw, -h, -hw);

  glm::vec3 nFront = glm::normalize(glm::cross(frontRight - frontLeft, tip - frontLeft));
  glm::vec3 nRight = glm::normalize(glm::cross(backRight - frontRight, tip - frontRight));
  glm::vec3 nBack = glm::normalize(glm::cross(backLeft - backRight, tip - backRight));
  glm::vec3 nLeft = glm::normalize(glm::cross(frontLeft - backLeft, tip - backLeft));
  glm::vec3 nBottom(0.0f, -1.0f, 0.0f);

  float positions[] = {
      // Front face
      frontLeft.x, frontLeft.y, frontLeft.z,
      frontRight.x, frontRight.y, frontRight.z,
      tip.x, tip.y, tip.z,
      // Right face
      frontRight.x, frontRight.y, frontRight.z,
      backRight.x, backRight.y, backRight.z,
      tip.x, tip.y, tip.z,
      // Back face
      backRight.x, backRight.y, backRight.z,
      backLeft.x, backLeft.y, backLeft.z,
      tip.x, tip.y, tip.z,
      // Left face
      backLeft.x, backLeft.y, backLeft.z,
      frontLeft.x, frontLeft.y, frontLeft.z,
      tip.x, tip.y, tip.z,
      // Bottom face 1
      frontLeft.x, frontLeft.y, frontLeft.z,
      backLeft.x, backLeft.y, backLeft.z,
      backRight.x, backRight.y, backRight.z,
      // Bottom face 2
      frontLeft.x, frontLeft.y, frontLeft.z,
      backRight.x, backRight.y, backRight.z,
      frontRight.x, frontRight.y, frontRight.z
  };

  float uvs[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f, // Front
      0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f, // Right
      0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f, // Back
      0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f, // Left
      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom 1
      0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f  // Bottom 2
  };

  float normals[] = {
      nFront.x, nFront.y, nFront.z, nFront.x, nFront.y, nFront.z, nFront.x, nFront.y, nFront.z,
      nRight.x, nRight.y, nRight.z, nRight.x, nRight.y, nRight.z, nRight.x, nRight.y, nRight.z,
      nBack.x, nBack.y, nBack.z, nBack.x, nBack.y, nBack.z, nBack.x, nBack.y, nBack.z,
      nLeft.x, nLeft.y, nLeft.z, nLeft.x, nLeft.y, nLeft.z, nLeft.x, nLeft.y, nLeft.z,
      nBottom.x, nBottom.y, nBottom.z, nBottom.x, nBottom.y, nBottom.z, nBottom.x, nBottom.y, nBottom.z,
      nBottom.x, nBottom.y, nBottom.z, nBottom.x, nBottom.y, nBottom.z, nBottom.x, nBottom.y, nBottom.z
  };

  uint32_t indices[] = {
      0, 1, 2,
      3, 4, 5,
      6, 7, 8,
      9, 10, 11,
      12, 13, 14,
      15, 16, 17
  };

  uploadData(positions, uvs, normals, indices);
}
