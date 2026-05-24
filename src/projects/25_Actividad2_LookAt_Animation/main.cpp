#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "engine/camera.hpp"
#include "engine/camera_controller.hpp"
#include "engine/geometry/cube.hpp"
#include "engine/geometry/sphere.hpp"
#include "engine/input.hpp"
#include "engine/shader.hpp"
#include "engine/texture.hpp"
#include "engine/window.hpp"

void render(const Shader &shader_light, const Shader &shader_phong, const Geometry &geo1, const Geometry &geo2, float time) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::vec3 lightPos(1.5f, 0.5f, 2.0f);
  glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

  glm::vec3 spherePos(0.0f, 0.0f, 0.0f);
  glm::vec3 sphereColor(0.7f, 0.4f, 0.2f);
  glm::vec3 cubeColor(0.2f, 0.2f, 0.8f);
  glm::vec3 cubePost(0.0f, 0.0f, 3.0f);

  // Rotate around the center using Camera.lookAt()
  glm::vec3 center(0.0f, 0.0f, 1.5f);
  float radius = 5.0f;
  float height = 2.0f;
  glm::vec3 cameraPos = center + glm::vec3(sin(time) * radius, height, cos(time) * radius);
  glm::mat4 view = Camera::lookAt(cameraPos, center, glm::vec3(0.0f, 1.0f, 0.0f));

  const glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), 0.1f, 100.0f);

  // Light ---------------------------------------
  {
    shader_light.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.05f));

    shader_light.set("model", model);
    shader_light.set("view", view);
    shader_light.set("proj", proj);

    shader_light.set("lightColor", lightColor);

    geo1.render();
  }

  // Sphere ---------------------------------------
  {
    shader_phong.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, spherePos);
    model = glm::scale(model, glm::vec3(0.7f));

    const glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(view * model)));

    shader_phong.set("model", model);
    shader_phong.set("normalMat", normalMat);
    shader_phong.set("view", view);
    shader_phong.set("proj", proj);

    shader_phong.set("light.position", glm::vec3(view * glm::vec4(lightPos, 1.0f)));
    shader_phong.set("light.ambient", lightColor * glm::vec3(0.1f));
    shader_phong.set("light.diffuse", lightColor * glm::vec3(0.8f));
    shader_phong.set("light.specular", glm::vec3(0.6f));

    shader_phong.set("material.ambient", sphereColor);
    shader_phong.set("material.diffuse", sphereColor);
    shader_phong.set("material.specular", sphereColor);
    shader_phong.set("material.shininess", 64);

    geo1.render();
  }

  // Cube ---------------------------------------
  {
    shader_phong.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cubePost);
    model = glm::scale(model, glm::vec3(0.1f));

    const glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(view * model)));

    shader_phong.set("model", model);
    shader_phong.set("normalMat", normalMat);
    shader_phong.set("view", view);
    shader_phong.set("proj", proj);

    shader_phong.set("light.position", glm::vec3(view * glm::vec4(lightPos, 1.0f)));
    shader_phong.set("light.ambient", lightColor * glm::vec3(0.1f));
    shader_phong.set("light.diffuse", lightColor * glm::vec3(0.8f));
    shader_phong.set("light.specular", glm::vec3(0.6f));

    shader_phong.set("material.ambient", cubeColor);
    shader_phong.set("material.diffuse", cubeColor);
    shader_phong.set("material.specular", cubeColor);
    shader_phong.set("material.shininess", 64);

    geo2.render();
  }
}

int main(int argc, char *argv[]) {
  Window *window = Window::instance();
  window->setCaptureMouse(false);  // Keyboard disabled
  Input *input = Input::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Sphere geo1 = Sphere(1.0f, 25, 25);
  const Cube geo2 = Cube(5.0f);
  const Shader shader_light( PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
  const Shader shader_phong(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  float lastFrame = 0.0f;

  while (window->isAlive()) {
    const auto currentFrame = glfwGetTime();
    const float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // handleInput(deltaTime); // Keyboard disabled.

    // update();
    render(shader_light, shader_phong, geo1, geo2, currentFrame);

    window->frame();
  }

  return 0;
}