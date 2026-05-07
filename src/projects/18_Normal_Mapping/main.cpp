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
#include "engine/geometry/cube.hpp"
#include "engine/geometry/quad.hpp"
#include "engine/geometry/sphere.hpp"

// Camera parameters
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
CameraController cameraController(camera);

void handleInput(const float deltaTime) {
  cameraController.handleInput(deltaTime);
}

void render(const Shader& shader_light, const Shader& shader_phong, const Shader& shader_normal,
  const Geometry& quad, const Geometry& sphere,
  const Texture& t_albedo, const Texture& t_specular, const Texture& t_normal) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

  glm::mat4 view = camera.getViewMatrix();
  const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), camera.getNear(), camera.getFar());

  float l_rotate[] = { std::sin((float)glfwGetTime()), 0.0f, std::abs(std::cos((float)glfwGetTime()))};
  glm::vec3 lightPos = glm::vec3(l_rotate[0], l_rotate[1], l_rotate[2]);

  // ---------------------------------------
  {
    shader_light.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.1f));

    shader_light.set("model", model);
    shader_light.set("view", view);
    shader_light.set("proj", proj);

    shader_light.set("lightColor", lightColor);

    sphere.render();
  }

  // ---------------------------------------  With Phong Blinn
  {
    shader_phong.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 0.0f ,0.0f));
    const glm::mat3 normalMat = glm::transpose(glm::inverse(model));

    shader_phong.set("model", model);
    shader_phong.set("normalMat", normalMat);
    shader_phong.set("view", view);
    shader_phong.set("proj", proj);

    shader_phong.set("viewPos", camera.getPosition());

    shader_phong.set("light.position", lightPos);
    shader_phong.set("light.ambient", lightColor * glm::vec3(0.1f));
    shader_phong.set("light.diffuse", lightColor * glm::vec3(0.8f));
    shader_phong.set("light.specular", glm::vec3(1.0f));

    t_albedo.use(shader_phong, "material.diffuse", 0);
    t_specular.use(shader_phong, "material.specular", 1);
    shader_phong.set("material.shininess", 64);    

    quad.render();
  }

  // --------------------------------------- With Normal Mapping

  {
    shader_normal.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
    const glm::mat3 normalMat = glm::transpose(glm::inverse(model));

    shader_normal.set("model", model);
    shader_normal.set("normalMat", normalMat);
    shader_normal.set("view", view);
    shader_normal.set("proj", proj);

    shader_normal.set("viewPos", camera.getPosition());

    shader_normal.set("light.position", lightPos);
    shader_normal.set("light.ambient", lightColor * glm::vec3(0.1f));
    shader_normal.set("light.diffuse", lightColor * glm::vec3(0.8f));
    shader_normal.set("light.specular", glm::vec3(1.0f));

    t_albedo.use(shader_normal, "material.diffuse", 0);
    t_specular.use(shader_normal, "material.specular", 1);
    t_normal.use(shader_normal, "material.normal", 2);
    shader_normal.set("material.shininess", 64);

    quad.render();
  }
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();
  window->setCaptureMouse(true);
  Input* input = Input::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Quad quad = Quad(2.0f);
  const Sphere sphere = Sphere(0.2f, 20, 20);
  const Texture t_albedo(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
  const Texture t_specular(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
  const Texture t_normal(ASSETS_PATH "textures/bricks_normal.png", Texture::Format::RGB);
  const Shader shader_light(PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
  const Shader shader_phong(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
  const Shader shader_normal(PROJECT_PATH "normal.vert", PROJECT_PATH "normal.frag");

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
    render(shader_light, shader_phong, shader_normal, quad, sphere, t_albedo, t_specular, t_normal);

    window->frame();
  }

  return 0;
}
