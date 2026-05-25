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
#include "engine/viewport.hpp"
#include "engine/geometry/quad.hpp"

// Camera parameters
Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));
CameraController cameraController(camera);

// SMALL CAMERA CONFIGURATION
glm::vec3 smallCamPos(5.0f, 5.0f, 5.0f);
glm::vec3 smallCamTarget(0.0f, 0.0f, 1.5f);
glm::vec3 smallCamUp(0.0f, 1.0f, 0.0f);
int smallCamWidth = 300;
int smallCamHeight = 200;
int smallCamX = 20;
int smallCamY = 20;

void handleInput(const float deltaTime) {
  cameraController.handleInput(deltaTime);
}

void render(const Shader &shader_light, const Shader &shader_phong, const Shader& shader_fbo, const Geometry &geo1, const Geometry &geo2, const Geometry &quad, float time) {
  int fbWidth, fbHeight;
  glfwGetFramebufferSize(Window::instance()->getNativeWindow(), &fbWidth, &fbHeight);

  auto draw = [&](const glm::mat4& view, const glm::mat4& proj) {
    glm::vec3 lightPos(1.5f, 0.5f, 2.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    glm::vec3 spherePos(0.0f, 0.0f, 0.0f);
    glm::vec3 sphereColor(0.7f, 0.4f, 0.2f);
    glm::vec3 cubeColor(0.2f, 0.2f, 0.8f);
    glm::vec3 cubePost(0.0f, 0.0f, 3.0f);

    // Light
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

    // Sphere
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

    // Cube
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
  };

  // PASS 1: Main Camera
  glViewport(0, 0, fbWidth, fbHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 mainView = camera.getViewMatrix();
  glm::mat4 mainProj = glm::perspective(glm::radians(camera.getFOV()), (float)fbWidth / (float)fbHeight, camera.getNear(), camera.getFar());
  draw(mainView, mainProj);

  // PASS 2: Small Camera (Render to FBO)
  Viewport* viewport = Viewport::instance();
  viewport->bindFBO();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 smallView = glm::lookAt(smallCamPos, smallCamTarget, smallCamUp);
  glm::mat4 smallProj = glm::perspective(glm::radians(45.0f), (float)smallCamWidth / (float)smallCamHeight, 0.1f, 100.0f);
  draw(smallView, smallProj);
  
  viewport->unbindFBO();

  // Restore the viewport to the default framebuffer size
  glViewport(0, 0, fbWidth, fbHeight);

  // PASS 3: Draw FBO to screen
  glDisable(GL_DEPTH_TEST);
  glViewport(smallCamX, smallCamY, smallCamWidth, smallCamHeight);

  shader_fbo.use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, viewport->getTexture());
  shader_fbo.set("screen_text", 0);
  quad.render();

  glEnable(GL_DEPTH_TEST);
}

int main(int argc, char *argv[]) {
  Window *window = Window::instance();
  window->setCaptureMouse(true);  // Keyboard enabled!
  Input *input = Input::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Sphere geo1 = Sphere(1.0f, 25, 25);
  const Cube geo2 = Cube(5.0f);
  const Quad quad = Quad(1.0f); // Use 1.0f so it precisely covers NDC [-1, 1]
  const Shader shader_light( PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
  const Shader shader_phong(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
  const Shader shader_fbo(PROJECT_PATH "fbo.vert", PROJECT_PATH "fbo.frag");

  Viewport* viewport = Viewport::instance();
  viewport->init(smallCamWidth, smallCamHeight);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Camera initial direction
  camera.lookAt(glm::vec3(0.0f, 0.0f, 1.5f));

  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  float lastFrame = 0.0f;

  while (window->isAlive()) {
    const auto currentFrame = glfwGetTime();
    const float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    handleInput(deltaTime); // Keyboard enabled!

    // update();
    render(shader_light, shader_phong, shader_fbo, geo1, geo2, quad, currentFrame);

    window->frame();
  }

  return 0;
}