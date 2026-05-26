#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/window.hpp"
#include "engine/shader.hpp"
#include "engine/camera.hpp"
#include "engine/camera_controller.hpp"
#include "engine/geometry/cube.hpp"
#include "engine/geometry/quad.hpp"
#include "engine/geometry/sphere.hpp"
#include "engine/geometry/pyramid.hpp"
#include "engine/oscillation.hpp"

// CAMERA CONSTANTS
const float CAMERA_SENSITIVITY = 0.2f;
const float CAMERA_SPEED = 10.0f;
const glm::vec3 CAMERA_POSITION(0.0f, 2.0f, 8.0f);

// QUAD / FLOOR CONSTANTS
const float QUAD_SIZE = 1.0f;
const glm::vec3 FLOOR_POSITION(0.0f, -1.0f, 0.0f);
const glm::vec3 FLOOR_SCALE(10.0f);
const glm::vec3 FLOOR_COLOR(1.0f, 1.0f, 1.0f); // White

// CUBE CONSTANTS
const float CUBE_SIZE = 1.0f;
const glm::vec3 CUBE_POSITION(-2.0f, -0.5f, 0.0f);
const glm::vec3 CUBE_SCALE(1.0f);
const glm::vec3 CUBE_COLOR(1.0f, 0.0f, 0.0f); // Red

// PYRAMID CONSTANTS
const float PYRAMID_SIZE = 1.0f;
const glm::vec3 PYRAMID_POSITION(2.0f, -0.5f, 0.0f);
const glm::vec3 PYRAMID_SCALE(1.0f);
const glm::vec3 PYRAMID_COLOR(0.0f, 0.0f, 1.0f); // Blue

// SPHERE CONSTANTS
const float SPHERE_RADIUS = 1.0f;
const int SPHERE_SECTORS = 20;
const int SPHERE_STACKS = 20;

// LIGHT CONSTANTS
const glm::vec3 POINT_LIGHT_FACETED_POSITION(0.0f, 3.0f, 0.0f);
const glm::vec3 POINT_LIGHT_FACETED_COLOR(1.0f, 1.0f, 1.0f);
const float POINT_LIGHT_FACETED_SPEED = 1.0f;
const float POINT_LIGHT_FACETED_RADIUS = 4.0f;
const float POINT_LIGHT_FACETED_INTENSITY = 3.0f;

Camera camera(CAMERA_POSITION);
CameraController cameraController(camera);

void handleInput(const float deltaTime) {
  cameraController.handleInput(deltaTime);
}

void drawQuad(const Shader& shader, const Quad& quad, const glm::mat4& view) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, FLOOR_POSITION);
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::scale(model, FLOOR_SCALE);
  shader.set("model", model);
  shader.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model))));
  shader.set("objectColor", FLOOR_COLOR);
  quad.render();
}

void drawCube(const Shader& shader, const Cube& cube, const glm::mat4& view) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, CUBE_POSITION);
  model = glm::scale(model, CUBE_SCALE);
  shader.set("model", model);
  shader.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model))));
  shader.set("objectColor", CUBE_COLOR);
  cube.render();
}

void drawPyramid(const Shader& shader, const Pyramid& pyramid, const glm::mat4& view) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, PYRAMID_POSITION);
  model = glm::scale(model, PYRAMID_SCALE);
  shader.set("model", model);
  shader.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model))));
  shader.set("objectColor", PYRAMID_COLOR);
  pyramid.render();
}

void drawPointLight(const Shader& shader_light, const Sphere& sphere, const glm::vec3& pos, const glm::vec3& color, float scale = 0.2f) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, pos);
  model = glm::scale(model, glm::vec3(scale));
  shader_light.set("model", model);
  shader_light.set("lightColor", color);
  sphere.render();
}

void render(const Shader& shader_light, const Shader& shader_phong, const Quad& quad, const Cube& cube, const Pyramid& pyramid, const Sphere& sphere, float time) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Oscillation lightOscillationFaceted(POINT_LIGHT_FACETED_POSITION, POINT_LIGHT_FACETED_RADIUS, glm::vec3(POINT_LIGHT_FACETED_SPEED, 0.0f, POINT_LIGHT_FACETED_SPEED));
  glm::vec3 currentLightPosFaceted = lightOscillationFaceted.getPosition(time);

  glm::mat4 view = camera.getViewMatrix();
  const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), camera.getNear(), camera.getFar());

  // Render Point Light
  shader_light.use();
  shader_light.set("view", view);
  shader_light.set("proj", proj);
  drawPointLight(shader_light, sphere, currentLightPosFaceted, POINT_LIGHT_FACETED_COLOR);

  // Render Scene
  shader_phong.use();
  shader_phong.set("view", view);
  shader_phong.set("proj", proj);

  // View Space: light position must be multiplied by view matrix
  shader_phong.set("lightFaceted.position", glm::vec3(view * glm::vec4(currentLightPosFaceted, 1.0f)));
  shader_phong.set("lightFaceted.ambient", POINT_LIGHT_FACETED_COLOR * POINT_LIGHT_FACETED_INTENSITY * 0.1f);
  shader_phong.set("lightFaceted.diffuse", POINT_LIGHT_FACETED_COLOR * POINT_LIGHT_FACETED_INTENSITY * 0.8f);
  shader_phong.set("lightFaceted.specular", POINT_LIGHT_FACETED_COLOR * POINT_LIGHT_FACETED_INTENSITY);
  shader_phong.set("lightFaceted.constant", 1.0f);
  shader_phong.set("lightFaceted.linear", 0.09f);
  shader_phong.set("lightFaceted.quadratic", 0.032f);

  drawQuad(shader_phong, quad, view);
  drawCube(shader_phong, cube, view);
  drawPyramid(shader_phong, pyramid, view);
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();
  window->setCaptureMouse(true);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  const Quad quad = Quad(QUAD_SIZE);
  const Cube cube = Cube(CUBE_SIZE);
  const Pyramid pyramid = Pyramid(PYRAMID_SIZE);
  const Sphere sphere = Sphere(SPHERE_RADIUS, SPHERE_SECTORS, SPHERE_STACKS);

  const Shader shader_light(PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
  const Shader shader_phong(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");

  camera.setMouseSensitivity(CAMERA_SENSITIVITY);
  camera.setMovementSpeed(CAMERA_SPEED);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  float lastFrame = 0.0f;

  while (window->isAlive()) {
    const float currentFrame = glfwGetTime();
    const float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    handleInput(deltaTime);
    render(shader_light, shader_phong, quad, cube, pyramid, sphere, currentFrame);
    window->frame();
  }

  return 0;
}
