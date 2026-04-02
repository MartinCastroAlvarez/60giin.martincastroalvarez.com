#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "engine/input.hpp"
#include "engine/window.hpp"
#include "engine/shader.hpp"
#include "engine/texture.hpp"
#include "engine/geometry/sphere.hpp"

std::vector<glm::vec3> cubePositions{
  glm::vec3(0.0f, 0.0f,0.0f),
  glm::vec3(1.0f, 6.0f, -20.0f),
  glm::vec3(-2.0f, -2.0f,-2.0f),
  glm::vec3(-4.0f, -6.0f,-10.0f),
  glm::vec3(3.0f, -1.0f,-4.0f),
  glm::vec3(-2.0f, 3.0f,-8.0f),
  glm::vec3(2.0f, -2.0f,-3.0f),
  glm::vec3(1.0f, 2.0f,-2.0f),
  glm::vec3(1.0f, 0.0f,-2.0f),
  glm::vec3(-1.0f, 1.0f,-1.0f)
};

// Camera parameters

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

const float cameraSpeed = 2.0f;
float fov = 45.0f;
const float near = 0.1f;
const float far = 50.0f;

float yaw = -90.0f;
float pitch = 0.0f;
float lastX, lastY;
bool firstMouse = true;

void onMouseMove(float x, float y) {
  if (firstMouse) {
    lastX = x;
    lastY = y;
    firstMouse = false;
  }

  float mouseSensitivity = 0.05f;
  float xOffset = (x - lastX) * mouseSensitivity;
  float yOffset = (lastY - y) * mouseSensitivity;
  lastX = x;
  lastY = y;  

  yaw += xOffset;
  pitch += yOffset;

  if (pitch > 75.0f)
    pitch = 75.0f;
  if (pitch < -75.0f)
    pitch = -75.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

  cameraFront = glm::normalize(direction);
}

void onScrollMove(float x, float y) {
  if (fov >= 10.0f && fov <= 45.0f)
    fov -= y;

  if (fov >= 45.0f)
    fov = 45.0f;

  if (fov <= 10.0f)
    fov = 10.0f;
}

void handleInput(const float deltaTime) {
  Input* input = Input::instance();
  const float speed = cameraSpeed * deltaTime;

  if (input->isKeyPressed(GLFW_KEY_W))
    cameraPos += speed * cameraFront;
  if (input->isKeyPressed(GLFW_KEY_S))
    cameraPos -= speed * cameraFront;
  if (input->isKeyPressed(GLFW_KEY_A))
    cameraPos -= speed * cameraRight;
  if (input->isKeyPressed(GLFW_KEY_D))
    cameraPos += speed * cameraRight;
}

void render(const Shader& shader, const Geometry& geo, const Texture& texture1, const Texture& texture2) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader.use();

  texture1.use(shader, "texture_1", 0);
  texture2.use(shader, "texture_2", 1);

  glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

  shader.set("view", view);

  const glm::mat4 proj = glm::perspective(glm::radians(fov), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), near, far);
  shader.set("proj", proj);

  for (size_t i = 0; i < cubePositions.size(); ++i) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cubePositions[i]);
    const float angle = 10.0f + (20.0f * static_cast<float>(i));
    model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

    shader.set("model", model);

    geo.render();
  }
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();
  window->setCaptureMouse(true);
  Input* input = Input::instance();

  input->setMouseMoveCallback(onMouseMove);
  input->setScrollMoveCallback(onScrollMove);

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Sphere geom = Sphere(2.0f, 25, 25);
  const Texture texture1 = Texture(ASSETS_PATH "textures/blue_blocks.jpg", Texture::Format::RGB);
  const Texture texture2 = Texture(ASSETS_PATH "textures/bricks_arrow.jpg", Texture::Format::RGB);
  const Shader shader(PROJECT_PATH "text.vert", PROJECT_PATH "text.frag");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  float lastFrame = 0.0f;

  while (window->isAlive()) {
    const auto currentFrame = glfwGetTime();
    const float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    handleInput(deltaTime);
    //update();
    render(shader, geom, texture1, texture2);

    window->frame();
  }

  return 0;
}