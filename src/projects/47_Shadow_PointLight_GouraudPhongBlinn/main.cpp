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
#include "engine/shadow.hpp"

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
const glm::vec3 POINT_LIGHT_GOURAUD_POSITION(0.0f, 3.0f, 0.0f);
const glm::vec3 POINT_LIGHT_GOURAUD_COLOR(1.0f, 1.0f, 1.0f);
const float POINT_LIGHT_GOURAUD_SPEED = 1.0f;
const float POINT_LIGHT_GOURAUD_RADIUS = 4.0f;
const float POINT_LIGHT_GOURAUD_INTENSITY = 3.0f;

Camera camera(CAMERA_POSITION);
CameraController cameraController(camera);

void handleInput(const float deltaTime) {
  cameraController.handleInput(deltaTime);
}

void drawQuad(const Shader& shader, const Quad& quad, const glm::mat4& view, bool isDepth = false) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, FLOOR_POSITION);
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::scale(model, FLOOR_SCALE);
  shader.set("model", model);
  shader.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model))));
  shader.set("objectColor", FLOOR_COLOR);
  quad.render();
}

void drawCube(const Shader& shader, const Cube& cube, const glm::mat4& view, bool isDepth = false) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, CUBE_POSITION);
  model = glm::scale(model, CUBE_SCALE);
  shader.set("model", model);
  if (!isDepth) {
    shader.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model))));
    shader.set("objectColor", CUBE_COLOR);
  }
  cube.render();
}

void drawPyramid(const Shader& shader, const Pyramid& pyramid, const glm::mat4& view, bool isDepth = false) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, PYRAMID_POSITION);
  model = glm::scale(model, PYRAMID_SCALE);
  shader.set("model", model);
  if (!isDepth) {
    shader.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model))));
    shader.set("objectColor", PYRAMID_COLOR);
  }
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

void render(const Shader& shader_light, const Shader& shader_phong, const Quad& quad, const Cube& cube, const Pyramid& pyramid, const Sphere& sphere, const Shader& shader_depth, float time) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Oscillation lightOscillationGouraud(POINT_LIGHT_GOURAUD_POSITION, POINT_LIGHT_GOURAUD_RADIUS, glm::vec3(POINT_LIGHT_GOURAUD_SPEED, 0.0f, POINT_LIGHT_GOURAUD_SPEED));
  glm::vec3 currentLightPosGouraud = lightOscillationGouraud.getPosition(time);

  glm::mat4 view = camera.getViewMatrix();
  const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), camera.getNear(), camera.getFar());

  glm::vec3 lightDirWorldGouraud = glm::normalize(-currentLightPosGouraud);
  Shadow* shadow = Shadow::instance();
  glm::mat4 lightSpaceMatrixGouraud = shadow->getLightSpaceMatrix(lightDirWorldGouraud);

  // PASS 1: DEPTH MAP
  shadow->bindFBO();
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  
  shader_depth.use();
  shader_depth.set("lightSpaceMatrixGouraud", lightSpaceMatrixGouraud);
  drawQuad(shader_depth, quad, view, true);
  drawCube(shader_depth, cube, view, true);
  drawPyramid(shader_depth, pyramid, view, true);

  // PASS 2: RENDER
  int fbWidth, fbHeight;
  glfwGetFramebufferSize(Window::instance()->getNativeWindow(), &fbWidth, &fbHeight);
  shadow->unbindFBO(fbWidth, fbHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render Point Light
  shader_light.use();
  shader_light.set("view", view);
  shader_light.set("proj", proj);
  drawPointLight(shader_light, sphere, currentLightPosGouraud, POINT_LIGHT_GOURAUD_COLOR);

  // Render Scene
  shader_phong.use();
  shader_phong.set("view", view);
  shader_phong.set("proj", proj);

  // View Space: light position must be multiplied by view matrix
  shader_phong.set("lightGouraud.position", glm::vec3(view * glm::vec4(currentLightPosGouraud, 1.0f)));
  shader_phong.set("lightGouraud.ambient", POINT_LIGHT_GOURAUD_COLOR * POINT_LIGHT_GOURAUD_INTENSITY * 0.1f);
  shader_phong.set("lightGouraud.diffuse", POINT_LIGHT_GOURAUD_COLOR * POINT_LIGHT_GOURAUD_INTENSITY * 0.8f);
  shader_phong.set("lightGouraud.specular", POINT_LIGHT_GOURAUD_COLOR * POINT_LIGHT_GOURAUD_INTENSITY);
  shader_phong.set("lightGouraud.constant", 1.0f);
  shader_phong.set("lightGouraud.linear", 0.09f);
  shader_phong.set("lightGouraud.quadratic", 0.032f);

  shader_phong.set("lightSpaceMatrixGouraud", lightSpaceMatrixGouraud);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, shadow->getDepthMap());
  shader_phong.set("shadowMapGouraud", 2);
  shader_phong.set("shadowIntensityGouraud", shadow->getIntensity());

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
  const Shader shader_depth(PROJECT_PATH "depth.vert", PROJECT_PATH "depth.frag");

  Shadow* shadow = Shadow::instance();
  shadow->init();
  shadow->setOrthoBoxSize(10.0f);
  shadow->setDistance(10.0f);
  shadow->setIntensity(0.8f);

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
    render(shader_light, shader_phong, quad, cube, pyramid, sphere, shader_depth, currentFrame);
    window->frame();
  }

  return 0;
}
