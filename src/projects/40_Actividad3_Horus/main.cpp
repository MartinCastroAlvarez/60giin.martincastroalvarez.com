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
#include "engine/geometry/quad.hpp"
#include "engine/geometry/sphere.hpp"
#include "engine/model.hpp"

// Camera parameters
Camera camera(glm::vec3(0.0f, 5.0f, 10.0f));
CameraController cameraController(camera);

glm::vec3 dirLightDirection(-0.4f, -1.0f, -0.5f);
glm::vec3 dirLightColor(0.4f, 0.4f, 0.4f);

glm::vec3 pointLightPosition0(0.0f, 2.0f, -4.0f);
glm::vec3 pointLightPosition1(0.0f, 2.0f, 4.0f);

glm::vec3 pointLightColor0(1.0f, 1.0f, 0.0f); // Pure Yellow
glm::vec3 pointLightColor1(1.0f, 0.8f, 0.2f); // Golden Yellow

glm::vec3 spotLightPosition0(4.0f, 2.0f, 0.0f);
glm::vec3 spotLightPosition1(-4.0f, 2.0f, 0.0f);

glm::vec3 spotLightDirection0(0.0f, -1.0f, 0.0f);
glm::vec3 spotLightDirection1(0.0f, -1.0f, 0.0f);

glm::vec3 spotLightColor0(0.6f, 0.0f, 0.8f); // Deep Purple
glm::vec3 spotLightColor1(0.8f, 0.5f, 1.0f); // Lavender / Light Purple

glm::vec3 flashlightColor(1.0f, 1.0f, 1.0f); // White flashlight

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

void render(const Shader& shader_light, const Shader& shader_phong, const Geometry& quad, const Model& horus, const Geometry& sphere, const Texture& texture_material, const Texture& texture_reflection, const Texture& texture_normal) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 view = camera.getViewMatrix();
  const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), camera.getNear(), camera.getFar());

  // ---------------------------------------
  {
    shader_light.use();

    shader_light.set("view", view);
    shader_light.set("proj", proj);

    // Point lights
    glm::mat4 model_point_light_0 = glm::mat4(1.0f);
    model_point_light_0 = glm::translate(model_point_light_0, pointLightPosition0);
    model_point_light_0 = glm::scale(model_point_light_0, glm::vec3(0.1f));
    shader_light.set("model", model_point_light_0);
    shader_light.set("lightColor", pointLightColor0);
    sphere.render();

    // Spot light
    glm::mat4 model_spot_light_0 = glm::mat4(1.0f);
    model_spot_light_0 = glm::translate(model_spot_light_0, spotLightPosition0);
    model_spot_light_0 = glm::scale(model_spot_light_0, glm::vec3(0.1f));
    shader_light.set("model", model_spot_light_0);
    shader_light.set("lightColor", spotLightColor0);
    sphere.render();
  }

  // ---------------------------------------
  {
    shader_phong.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f ,0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f));

    const glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(view * model)));

    shader_phong.set("model", model);
    shader_phong.set("normalMat", normalMat);
    shader_phong.set("view", view);
    shader_phong.set("proj", proj);

    texture_material.use(shader_phong, "texture_diffuse1", 0);
    texture_reflection.use(shader_phong, "texture_specular1", 1);
    texture_normal.use(shader_phong, "texture_normal1", 2); // NORMAL MAPPING
    shader_phong.set("shininess", 64);
    shader_phong.set("normalIntensity", 1.0f);

    // Directional Light ---------------------------------------

    shader_phong.set("dirLight.direction", glm::vec3(view * glm::vec4(dirLightDirection, 0.0f)));
    shader_phong.set("dirLight.ambient", dirLightColor * glm::vec3(0.1f));
    shader_phong.set("dirLight.diffuse", dirLightColor * glm::vec3(0.8f));
    shader_phong.set("dirLight.specular", glm::vec3(1.0f));

    // Point Light 0

    shader_phong.set("pointLight0.position", glm::vec3(view * glm::vec4(pointLightPosition0, 1.0f)));

    shader_phong.set("pointLight0.ambient", pointLightColor0 * glm::vec3(0.1f));
    shader_phong.set("pointLight0.diffuse", pointLightColor0 * glm::vec3(0.8f));
    shader_phong.set("pointLight0.specular", glm::vec3(1.0f));
    shader_phong.set("pointLight0.constant", 1.0f);
    shader_phong.set("pointLight0.linear", 0.14f);
    shader_phong.set("pointLight0.quadratic", 0.07f);

    // Spot Light 0

    shader_phong.set("spotLight0.position", glm::vec3(view * glm::vec4(spotLightPosition0, 1.0f)));
    shader_phong.set("spotLight0.direction", glm::vec3(view * glm::vec4(spotLightDirection0, 0.0f)));

    shader_phong.set("spotLight0.ambient", spotLightColor0 * glm::vec3(0.1f));
    shader_phong.set("spotLight0.diffuse", spotLightColor0 * glm::vec3(0.8f));
    shader_phong.set("spotLight0.specular", glm::vec3(1.0f));
    shader_phong.set("spotLight0.constant", 1.0f);
    shader_phong.set("spotLight0.linear", 0.14f);
    shader_phong.set("spotLight0.quadratic", 0.07f);

    // CONOS DIFUSOS (SOFT CUT OFF)
    shader_phong.set("spotLight0.cutOff", glm::cos(glm::radians(30.0f)));
    shader_phong.set("spotLight0.outerCutOff", glm::cos(glm::radians(35.0f)));

    quad.render();
  }

  // HORUS MODEL
  glm::mat4 horusModelMat = glm::mat4(1.0f);
  horusModelMat = glm::translate(horusModelMat, glm::vec3(0.0f, -1.0f, 0.0f));
  horusModelMat = glm::rotate(horusModelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  horusModelMat = glm::scale(horusModelMat, glm::vec3(0.01f)); // Adjust scale if needed
  shader_phong.set("model", horusModelMat);
  shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * horusModelMat))));
  horus.render(shader_phong);
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();
  window->setCaptureMouse(true);
  Input* input = Input::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Quad quad = Quad(1.0f);
  const Sphere sphere = Sphere(1.0f, 20, 20);
  const Model horus(ASSETS_PATH "models/Horus/Horus_VRay.FBX");
  const Texture texture_material(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
  const Texture texture_reflection(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
  const Texture texture_normal(ASSETS_PATH "textures/bricks_normal.png", Texture::Format::RGB);
  const Shader shader_light(PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
  const Shader shader_phong(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // Camera initial direction
  camera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

  float lastFrame = 0.0f;

  while (window->isAlive()) {
    const auto currentFrame = glfwGetTime();
    const float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    handleInput(deltaTime);
    //update();
    render(shader_light, shader_phong, quad, horus, sphere, texture_material, texture_reflection, texture_normal);

    window->frame();
  }

  return 0;
}
