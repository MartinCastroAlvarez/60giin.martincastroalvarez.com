#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "engine/window.hpp"
#include "engine/shader.hpp"

void handleInput() {}

uint32_t createTexture(const char* path) {
  uint32_t texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nChannels;
  unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    std::cout << "Texture loaded succesfully " << path << " size(" << width << "x" << height << ")" << std::endl;
    stbi_image_free(data);
  }
  else {
    std::cout << "Error loading texture" << std::endl;
  }
  
  return texture;
}

std::tuple<uint32_t, uint32_t, uint32_t> createVertexData() {
  float vertices[] = {
    // positions         // colors             // texture coords
    0.5f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,     1.0f, 1.0f,   //upper right
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   //down right
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,     0.0f, 0.0f,   //down left
    -0.5f, 0.5f, 0.0f,   1.0f, 1.0f, 0.0f,     0.0f, 1.0f    //upper left
  };

  uint32_t indices[] = { 0, 3, 1,
                         1, 3, 2};

  uint32_t VAO;
  glGenVertexArrays(1, &VAO);  // Create VAO
  glBindVertexArray(VAO);  // Bind VAO

  uint32_t EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  uint32_t VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);  // Unbind VAO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return std::make_tuple(VAO, VBO, EBO);
}

void render(const Shader& shader, const uint32_t VAO, uint32_t texture1, uint32_t texture2) {
  glClear(GL_COLOR_BUFFER_BIT);

  shader.use();
  glBindVertexArray(VAO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture1);
  shader.set("texture_1", 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture2);
  shader.set("texture_2", 1);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  auto vertexData = createVertexData();
  auto texture1= createTexture(ASSETS_PATH "textures/blue_blocks.jpg");
  auto texture2 = createTexture(ASSETS_PATH "textures/bricks_arrow.jpg");
  const Shader shader(PROJECT_PATH "text.vert", PROJECT_PATH "text.frag");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  while (window->isAlive()) {
    handleInput();
    //update();
    render(shader, std::get<0>(vertexData), texture1, texture2);

    window->frame();
  }

  glDeleteVertexArrays(1, &std::get<0>(vertexData));
  glDeleteBuffers(1, &std::get<1>(vertexData));
  glDeleteBuffers(1, &std::get<2>(vertexData));
  glDeleteTextures(1, &texture1);
  glDeleteTextures(1, &texture2);

  return 0;
}