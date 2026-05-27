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
#include "engine/oscillation.hpp"

const glm::vec4 BACKGROUND_COLOR(0.1f, 0.1f, 0.1f, 1.0f);

// Camera parameters
const float CAMERA_SENSITIVITY = 0.2f;
const float CAMERA_SPEED = 10.0f;
const glm::vec3 CAMERA_POSITION(0.0f, 5.0f, -30.0f);

// Directional light parameters.
glm::vec3 dirLightDirection(-0.4f, -1.0f, -0.5f);
glm::vec3 dirLightColor(0.4f, 0.4f, 0.4f);

// Point light parameters.
glm::vec3 pointLightPosition0(0.0f, 0.0f, 0.0f);
glm::vec3 pointLightSpeed0(1.0f, 1.0f, 1.0f);
float pointLightRadius0 = 5;
glm::vec3 pointLightColor0(2.0f, 2.0f, 2.0f); // Pure Yellow

// Big sphere parameters.
glm::vec3 bigSpherePosition0(-10.0f, -0.5f, 0.0f);
glm::vec3 bigSpherePosition1(0.0f, -0.5f, 0.0f);
glm::vec3 bigSpherePosition2(10.0f, -0.5f, 0.0f);
float bigSphereSize = 4;
glm::vec3 bigSphereColor(0.5f, 0.0f, 1.0f);

Camera camera(CAMERA_POSITION);
CameraController cameraController(camera);

void handleInput(const float deltaTime) {
  cameraController.handleInput(deltaTime);
}

void render(const Shader& shader_light, const Shader& shader_phong, const Geometry& sphere, const Texture& texture_material, const Texture& texture_reflection, float time, const Sphere& bigSphere) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Oscillation movement
  Oscillation pointLightOscillation0(pointLightPosition0, pointLightRadius0, pointLightSpeed0);
  glm::vec3 pointLightEffectivePosition0 = pointLightOscillation0.getPosition(time);

  // Camera
  glm::mat4 view = camera.getViewMatrix();
  const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), camera.getNear(), camera.getFar());

  // ---------------------------------------
  {
    shader_light.use();

    shader_light.set("view", view);
    shader_light.set("proj", proj);

    // Point lights
    glm::mat4 model_point_light_0 = glm::mat4(1.0f);
    model_point_light_0 = glm::translate(model_point_light_0, pointLightEffectivePosition0);
    model_point_light_0 = glm::scale(model_point_light_0, glm::vec3(0.1f));
    shader_light.set("model", model_point_light_0);
    shader_light.set("lightColor", pointLightColor0);
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

    texture_material.use(shader_phong, "material.diffuse", 0);
    texture_reflection.use(shader_phong, "material.specular", 1);
    shader_phong.set("material.shininess", 64);

    // Directional Light
    shader_phong.set("dirLight.direction", glm::vec3(view * glm::vec4(dirLightDirection, 0.0f)));
    shader_phong.set("dirLight.ambient", dirLightColor * glm::vec3(0.1f));
    shader_phong.set("dirLight.diffuse", dirLightColor * glm::vec3(0.8f));
    shader_phong.set("dirLight.specular", glm::vec3(1.0f));

    // Point Light
    shader_phong.set("pointLight0.position", glm::vec3(view * glm::vec4(pointLightEffectivePosition0, 1.0f)));
    shader_phong.set("pointLight0.ambient", pointLightColor0 * glm::vec3(0.1f));
    shader_phong.set("pointLight0.diffuse", pointLightColor0 * glm::vec3(0.8f));
    shader_phong.set("pointLight0.specular", glm::vec3(1.0f));
    shader_phong.set("pointLight0.constant", 1.0f);
    shader_phong.set("pointLight0.linear", 0.14f);
    shader_phong.set("pointLight0.quadratic", 0.07f);

    // Big Spheres ---------------------------------------
    glm::mat4 m0 = glm::mat4(1.0f);
    m0 = glm::translate(m0, bigSpherePosition0);
    m0 = glm::scale(m0, glm::vec3(0.5f));
    shader_phong.set("model", m0);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * m0))));
    bigSphere.render();

    glm::mat4 m1 = glm::mat4(1.0f);
    m1 = glm::translate(m1, bigSpherePosition1);
    m1 = glm::scale(m1, glm::vec3(0.5f));
    shader_phong.set("model", m1);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * m1))));
    shader_phong.set("objectColor", bigSphereColor);
    bigSphere.render();

    glm::mat4 m2 = glm::mat4(1.0f);
    m2 = glm::translate(m2, bigSpherePosition2);
    m2 = glm::scale(m2, glm::vec3(0.5f));
    shader_phong.set("model", m2);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * m2))));
    bigSphere.render();
  }
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();
  window->setCaptureMouse(true);
  Input* input = Input::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Sphere sphere = Sphere(1.0f, 20, 20);
  const Sphere bigSphere = Sphere(bigSphereSize, 100, 20);
  const Texture texture_material(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
  const Texture texture_reflection(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
  const Shader shader_light(PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
  const Shader shader_phong(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // Camera initial direction
  camera.setMouseSensitivity(CAMERA_SENSITIVITY);
  camera.setMovementSpeed(CAMERA_SPEED);
  camera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

  float lastFrame = 0.0f;

  while (window->isAlive()) {
    const auto currentFrame = glfwGetTime();
    const float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    handleInput(deltaTime);
    //update();
    render(shader_light, shader_phong, sphere, texture_material, texture_reflection, currentFrame, bigSphere);

    window->frame();
  }

  return 0;
}
