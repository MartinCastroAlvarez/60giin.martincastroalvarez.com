#include <GLFW/glfw3.h>

#include "engine/camera_controller.hpp"
#include "engine/input.hpp"
#include "engine/camera.hpp"

CameraController::CameraController(Camera& camera) 
  : camera_(camera) {
  Input* input = Input::instance();

  input->setMouseMoveCallback([=](float a, float b) {
    this->onMouseMove(a, b);
    });

  input->setScrollMoveCallback([=](float a, float b) {
    this->onScrollMove(a, b);
    });
};

void CameraController::handleInput(float deltaTime) {
  Input* input = Input::instance();

  if (input->isKeyPressed(GLFW_KEY_W))
    camera_.handleKeyboard(Camera::Movement::Forward, deltaTime);
  if (input->isKeyPressed(GLFW_KEY_S))
    camera_.handleKeyboard(Camera::Movement::Backward, deltaTime);
  if (input->isKeyPressed(GLFW_KEY_A))
    camera_.handleKeyboard(Camera::Movement::Left, deltaTime);
  if (input->isKeyPressed(GLFW_KEY_D))
    camera_.handleKeyboard(Camera::Movement::Right, deltaTime);
}

void CameraController::onMouseMove(float x, float y) {
  if (firstMouse_) {
    lastX_ = x;
    lastY_ = y;
    firstMouse_ = false;
  }

  float xoffset = x - lastX_;
  float yoffset = lastY_ - y;
  lastX_ = x;
  lastY_ = y;

  camera_.handleMouseMovement(xoffset, yoffset);
}

void CameraController::onScrollMove(float xOffset, float yOffset) {
  camera_.handleMouseScroll(yOffset);
}