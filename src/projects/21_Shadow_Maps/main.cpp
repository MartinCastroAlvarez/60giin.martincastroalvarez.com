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
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
CameraController cameraController(camera);

void handleInput(const float deltaTime) {
  cameraController.handleInput(deltaTime);
}

const uint32_t k_shadow_width = 2048;
const uint32_t k_shadow_height = 2048;

const float k_shadow_near = 1.0f;
const float k_shadow_far = 7.5f;

glm::vec3 lightPos(1.5f, 4.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

std::pair<uint32_t, uint32_t> createFBO() {
  uint32_t fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  uint32_t depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, k_shadow_width, k_shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Error Framebuffer not complete" << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return std::make_pair(fbo, depthMap);
}

void renderScene(const Shader& shader,
  const Geometry& quad, const Geometry& cube, const Geometry& sphere) {

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::scale(model, glm::vec3(10.0f));
  glm::mat3 normalMat = glm::transpose(glm::inverse(model));

  shader.set("model", model);
  shader.set("normalMat", normalMat);
  quad.render();


  model = glm::mat4(1.0f);
  normalMat = glm::transpose(glm::inverse(model));

  shader.set("model", model);
  shader.set("normalMat", normalMat);
  cube.render();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(3.0f, 0.5f, 0.0f));
  normalMat = glm::transpose(glm::inverse(model));

  shader.set("model", model);
  shader.set("normalMat", normalMat);
  cube.render();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 1.5f, 1.0f));
  model = glm::scale(model, glm::vec3(0.5f));
  normalMat = glm::transpose(glm::inverse(model));

  shader.set("model", model);
  shader.set("normalMat", normalMat);
  sphere.render();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(2.0f, 1.0f, -1.0f));
  model = glm::scale(model, glm::vec3(0.3f));
  normalMat = glm::transpose(glm::inverse(model));

  shader.set("model", model);
  shader.set("normalMat", normalMat);
  sphere.render();
}

void configureShader(const Shader& shader, const Texture& t_albedo, const Texture& t_specular) {
  glm::mat4 view = camera.getViewMatrix();
  const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), camera.getNear(), camera.getFar());

  shader.use();
  shader.set("view", view);
  shader.set("proj", proj);

  shader.set("viewPos", camera.getPosition());

  shader.set("light.position", lightPos);
  shader.set("light.ambient", lightColor * glm::vec3(0.1f));
  shader.set("light.diffuse", lightColor * glm::vec3(0.8f));
  shader.set("light.specular", glm::vec3(1.0f));

  t_albedo.use(shader, "material.diffuse", 0);
  t_specular.use(shader, "material.specular", 1);
  shader.set("material.shininess", 64);
}

void render(const Shader& shader_phong, const Shader& shader_depth, const Shader& shader_debug, const Shader& shader_light,
  const Geometry& quad, const Geometry& cube, const Geometry& sphere,
  const Texture& t_albedo, const Texture& t_specular,
  const uint32_t fbo, const uint32_t text_fbo) {  

  glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, k_shadow_near, k_shadow_far);
  glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 lightSpaceMatrix = lightProjection * lightView;

  //FIRST PASS DEPTH MAP FROM LIGHT
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, k_shadow_width, k_shadow_height);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  //glCullFace(GL_FRONT);

  shader_depth.use();
  shader_depth.set("lightSpaceMatrix", lightSpaceMatrix);

  renderScene(shader_depth, quad, cube, sphere);

  //DEBUG PASS

  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
  //glClear(GL_COLOR_BUFFER_BIT);
  //glDisable(GL_DEPTH_TEST);

  //shader_debug.use();
  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_2D, text_fbo),
  //shader_debug.set("depth_map", 0);

  //quad.render();

  // SECOND PASS - Render Scene
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
  glEnable(GL_DEPTH_TEST);  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glCullFace(GL_BACK);

  // Render Light

  configureShader(shader_light, t_albedo, t_specular);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, lightPos);
  model = glm::scale(model, glm::vec3(0.05f));

  shader_light.set("model", model);
  sphere.render();

  // Render Scene

  configureShader(shader_phong, t_albedo, t_specular);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, text_fbo),
  shader_phong.set("depthMap", 2);

  shader_phong.set("lightSpaceMatrix", lightSpaceMatrix);

  renderScene(shader_phong, quad, cube, sphere);
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();
  window->setCaptureMouse(true);

  Input* input = Input::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Cube cube = Cube(1.0f);
  const Quad quad = Quad(2.0f);
  const Sphere sphere(1.0f, 20, 20);
  const Texture t_albedo(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
  const Texture t_specular(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
  const Shader shader_phong(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
  const Shader shader_depth(PROJECT_PATH "depth.vert", PROJECT_PATH "depth.frag");
  const Shader shader_debug(PROJECT_PATH "debug.vert", PROJECT_PATH "debug.frag");
  const Shader shader_light(PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");

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
    render(shader_phong, shader_depth, shader_debug, shader_light, quad, cube, sphere, t_albedo, t_specular, fbo.first, fbo.second);

    window->frame();
  }

  glDeleteFramebuffers(1, &fbo.first);
  glDeleteTextures(1, &fbo.second);

  return 0;
};