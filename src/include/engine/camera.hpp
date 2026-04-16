#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Default camera values
const float k_Yaw = -90.0f;
const float k_Pitch = 0.0f;
const float k_Speed = 1.0f;
const float k_Sensitivity = 0.05f;
const float k_FOV = 45.0f;
const float k_Near = 0.1f;
const float k_Far = 45.0f;

class Camera {
public:
  enum class Movement {  // possible options for camera movement
    Forward = 0,
    Backward = 1,
    Left = 2,
    Right = 3,
  };

  // Constructor with vectors
  Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = k_Yaw, float pitch = k_Pitch);

  // Constructor with scalars
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

  [[nodiscard]] glm::mat4 getViewMatrix() const;
  [[nodiscard]] float getFOV() const;
  [[nodiscard]] glm::vec3 getPosition() const;
  [[nodiscard]] float getNear() const;
  [[nodiscard]] float getFar() const;

  void handleKeyboard(Movement direction, float dt);
  void handleMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
  void handleMouseScroll(float yoffset);

private:
  void updateCameraVectors();

  glm::vec3 position_, front_, up_, right_, worldUp_;
  float yaw_, pitch_;
  float fov_;
  float near_ = k_Near;
  float far_ = k_Far;
};