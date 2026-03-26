#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "engine/window.hpp"
#include "engine/shader.hpp"
#include "engine/texture.hpp"
#include "engine/geometry/teapot.hpp"

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

void handleInput() {}

void render(const Shader& shader, const Geometry& geo, const Texture& texture1, const Texture& texture2) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader.use();

  texture1.use(shader, "texture_1", 0);
  texture2.use(shader, "texture_2", 1);

  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  shader.set("view", view);

  float fov = 45.0f;
  float near = 0.1f;
  float far = 50.0f;
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

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Teapot geom = Teapot(20);
  const Texture texture1 = Texture(ASSETS_PATH "textures/blue_blocks.jpg", Texture::Format::RGB);
  const Texture texture2 = Texture(ASSETS_PATH "textures/bricks_arrow.jpg", Texture::Format::RGB);
  const Shader shader(PROJECT_PATH "text.vert", PROJECT_PATH "text.frag");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  while (window->isAlive()) {
    handleInput();
    //update();
    render(shader, geom, texture1, texture2);

    window->frame();
  }

  return 0;
}