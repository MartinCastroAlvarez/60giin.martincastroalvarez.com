#include "engine/camera.hpp"

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch) {
  position_ = position;
  worldUp_ = up;
  yaw_ = yaw;
  pitch_ = pitch;
  fov_ = k_FOV;
  updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) {
  position_ = glm::vec3(posX, posY, posZ);
  worldUp_ = glm::vec3(upX, upY, upZ);
  yaw_ = yaw;
  pitch_ = pitch;
  fov_ = k_FOV;
  updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
  return Camera::lookAt(position_, position_ + front_, worldUp_);
}

glm::mat4 Camera::lookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& worldUp) {
  glm::vec3 zaxis = glm::normalize(position - target);
  glm::vec3 xaxis = glm::normalize(glm::cross(worldUp, zaxis));
  glm::vec3 yaxis = glm::cross(zaxis, xaxis);

  glm::mat4 translation = glm::mat4(1.0f);
  translation[3][0] = -position.x;
  translation[3][1] = -position.y;
  translation[3][2] = -position.z;

  glm::mat4 rotation = glm::mat4(1.0f);
  rotation[0][0] = xaxis.x;
  rotation[1][0] = xaxis.y;
  rotation[2][0] = xaxis.z;
  rotation[0][1] = yaxis.x;
  rotation[1][1] = yaxis.y;
  rotation[2][1] = yaxis.z;
  rotation[0][2] = zaxis.x;
  rotation[1][2] = zaxis.y;
  rotation[2][2] = zaxis.z;

  return rotation * translation;
}

float Camera::getFOV() const {
  return fov_;
}

glm::vec3 Camera::getPosition() const {
  return position_;
}

glm::vec3 Camera::getDirection() const {
  return front_;
}

float Camera::getNear() const {
  return near_;
}

float Camera::getFar() const {
  return far_;
}

void Camera::updateCameraVectors() {
  glm::vec3 front;   // calculate the direction vector
  front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  front.y = sin(glm::radians(pitch_));
  front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  front_ = glm::normalize(front);

  right_ = glm::normalize(glm::cross(front_, worldUp_));
  up_ = glm::normalize(glm::cross(right_, front_));
}

void Camera::handleKeyboard(Movement direction, float dt) {
  const float velocity = movementSpeed_ * dt;

  switch(direction) {
    case Movement::Forward:     position_ += front_ * velocity;  break;
    case Movement::Backward:    position_ -= front_ * velocity;  break;
    case Movement::Left:        position_ -= right_ * velocity;  break;
    case Movement::Right:       position_ += right_ * velocity;  break;
    case Movement::Up:          position_ += up_ * velocity;     break;
    case Movement::Down:        position_ -= up_ * velocity;     break;
  }
}

void Camera::handleMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
  float xoff = xoffset * mouseSensitivity_;
  float yoff = yoffset * mouseSensitivity_;

  yaw_ += xoff;
  pitch_ += yoff;

  if (constrainPitch) {
    if (pitch_> 89.0f) pitch_ = 89.0f;
    if (pitch_ < -89.0f) pitch_ = -89.0f;
  }

  updateCameraVectors();
}

void Camera::handleMouseScroll(float yoffset) {
  if (fov_ >= 1.0f && fov_ <= 45.0f) fov_ -= yoffset;
  if (fov_ <= 1.0f) fov_ = 1.0f;
  if (fov_ >= 45.0f) fov_ = 45.0f;
}

void Camera::lookAt(const glm::vec3& target) {
  glm::vec3 direction = glm::normalize(target - position_);
  pitch_ = glm::degrees(asin(direction.y));
  yaw_ = glm::degrees(atan2(direction.z, direction.x));
  updateCameraVectors();
}