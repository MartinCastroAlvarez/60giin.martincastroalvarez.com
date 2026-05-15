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

// Camera parameters
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
CameraController cameraController(camera);

std::vector<glm::vec3> cubePositions{
  glm::vec3(0.0f, -0.5f, -4.0f),
  glm::vec3(0.0f, -0.5f, 4.0f),
  glm::vec3(-4.0f, -0.5f, -4.0f),
  glm::vec3(-4.0f, -0.5f, 4.0f),
  glm::vec3(-4.0f, -0.5f, 0.0f),
  glm::vec3(4.0f, -0.5f, -4.0f),
  glm::vec3(4.0f, -0.5f, 4.0f),
  glm::vec3(4.0f, -0.5f, 0.0f)
};

void handleInput(const float deltaTime) {
  cameraController.handleInput(deltaTime);
}

std::pair<uint32_t, uint32_t> createFBO() {
  uint32_t fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  uint32_t textColor;
  glGenTextures(1, &textColor);
  glBindTexture(GL_TEXTURE_2D, textColor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::instance()->getWidth(), Window::instance()->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textColor, 0);

  uint32_t rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Window::instance()->getWidth(), Window::instance()->getHeight());
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Error Framebuffer not complete" << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return std::make_pair(fbo, textColor);
}

void render(const Shader& shader_phong, const Shader& shader_fbo, 
  const Geometry& quad, const Geometry& cube,
  const Texture& t_albedo, const Texture& t_specular,
  const uint32_t fbo, const uint32_t text_fbo) {  

  glm::vec3 lightDir(-0.4f, -1.0f, -0.5f);
  glm::vec3 lightColor(0.0f, 0.0f, 1.0f);

  glm::mat4 view = camera.getViewMatrix();
  const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), camera.getNear(), camera.getFar());

  // --------------------------------------- FIRST PASS

  {
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_phong.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f ,0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f));

    const glm::mat3 normalMat = glm::transpose(glm::inverse(model));

    shader_phong.set("model", model);
    shader_phong.set("normalMat", normalMat);
    shader_phong.set("view", view);
    shader_phong.set("proj", proj);

    shader_phong.set("viewPos", camera.getPosition());

    shader_phong.set("light.direction", lightDir);
    shader_phong.set("light.ambient", lightColor * glm::vec3(0.1f));
    shader_phong.set("light.diffuse", lightColor * glm::vec3(0.8f));
    shader_phong.set("light.specular", glm::vec3(1.0f));

    t_albedo.use(shader_phong, "material.diffuse", 0);
    t_specular.use(shader_phong, "material.specular", 1);
    shader_phong.set("material.shininess", 64);    

    quad.render();
  }

  // ---------------------------------------

  for (auto& pos : cubePositions) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    const glm::mat3 normalMat = glm::transpose(glm::inverse(model));

    shader_phong.set("model", model);
    shader_phong.set("normalMat", normalMat);

    cube.render();
  }

  // --------------------------------------- SECOND PASS

  {
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    shader_fbo.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text_fbo);
    shader_fbo.set("screen_text", 0);

    quad.render();
  }
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();
  window->setCaptureMouse(true);

  Input* input = Input::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Cube cube = Cube(1.0f);
  const Quad quad = Quad(2.0f);
  const Texture t_albedo(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
  const Texture t_specular(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
  const Shader shader_phong(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
  const Shader shader_fbo(PROJECT_PATH "fbo.vert", PROJECT_PATH "fbo.frag");

  auto fbo = createFBO();

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  float lastFrame = 0.0f;

  while (window->isAlive()) {
    const auto currentFrame = glfwGetTime();
    const float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    handleInput(deltaTime);
    //update();
    render(shader_phong, shader_fbo, quad, cube, t_albedo, t_specular, fbo.first, fbo.second);

    window->frame();
  }

  glDeleteFramebuffers(1, &fbo.first);
  glDeleteTextures(1, &fbo.second);

  return 0;
};