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
#include "engine/camera.hpp"
#include "engine/camera_controller.hpp"
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
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
CameraController cameraController(camera);


void handleInput(const float deltaTime) {
  cameraController.handleInput(deltaTime);
}

void render(const Shader& shader_light, const Shader& shader_phong, const Geometry& geo) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::vec3 lightPos(0.5f, 0.5f, 2.0f);
  glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

  glm::vec3 objectPos(0.0f, 0.0f, 0.0f);
  glm::vec3 objectColor(0.7f, 0.4f, 0.2f);

  glm::mat4 view = camera.getViewMatrix();
  const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), camera.getNear(), camera.getFar());

  // ---------------------------------------
  {
    shader_light.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.05f));

    shader_light.set("model", model);
    shader_light.set("view", view);
    shader_light.set("proj", proj);

    shader_light.set("lightColor", lightColor);

    geo.render();
  }

  // ---------------------------------------

  {
    shader_phong.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, objectPos);
    model = glm::scale(model, glm::vec3(0.7f));

    const glm::mat3 normalMat = glm::transpose(glm::inverse(model));

    shader_phong.set("model", model);
    shader_phong.set("normalMat", normalMat);
    shader_phong.set("view", view);
    shader_phong.set("proj", proj);

    shader_phong.set("light.position", lightPos);
    shader_phong.set("light.ambient", lightColor * glm::vec3(0.1f));
    shader_phong.set("light.diffuse", lightColor * glm::vec3(0.8f));
    shader_phong.set("light.specular", glm::vec3(0.6f));

    shader_phong.set("material.ambient", objectColor);
    shader_phong.set("material.diffuse", objectColor);
    shader_phong.set("material.specular", objectColor);
    shader_phong.set("material.shininess", 64);

    shader_phong.set("viewPos", camera.getPosition());

    geo.render();
  }
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();
  window->setCaptureMouse(true);
  Input* input = Input::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Sphere geom = Sphere(1.0f, 25, 25);
  const Shader shader_light(PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
  const Shader shader_phong(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");

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
    render(shader_light, shader_phong, geom);

    window->frame();
  }

  return 0;
}