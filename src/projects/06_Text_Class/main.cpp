#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "engine/window.hpp"
#include "engine/shader.hpp"
#include "engine/texture.hpp"
#include "engine/geometry/quad.hpp"

void handleInput() {}

void render(const Shader& shader, const Geometry& geo, const Texture& texture1, const Texture& texture2) {
  glClear(GL_COLOR_BUFFER_BIT);

  shader.use(); 

  texture1.use(shader, "texture_1", 0);
  texture2.use(shader, "texture_2", 1);

  geo.render();
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Quad quad = Quad(1.0f);
  const Texture texture1 = Texture(ASSETS_PATH "textures/blue_blocks.jpg", Texture::Format::RGB);
  const Texture texture2 = Texture(ASSETS_PATH "textures/bricks_arrow.jpg", Texture::Format::RGB);
  const Shader shader(PROJECT_PATH "text.vert", PROJECT_PATH "text.frag");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  while (window->isAlive()) {
    handleInput();
    //update();
    render(shader, quad, texture1, texture2);

    window->frame();
  }

  return 0;
}