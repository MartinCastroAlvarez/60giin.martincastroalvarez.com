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

glm::vec3 cubePosition0(0.0f, -0.5f, -4.0f);
glm::vec3 cubePosition1(0.0f, -0.5f, 4.0f);
glm::vec3 cubePosition2(-4.0f, -0.5f, -4.0f);
glm::vec3 cubePosition3(-4.0f, -0.5f, 4.0f);
glm::vec3 cubePosition4(-4.0f, -0.5f, 0.0f);
glm::vec3 cubePosition5(4.0f, -0.5f, -4.0f);
glm::vec3 cubePosition6(4.0f, -0.5f, 4.0f);
glm::vec3 cubePosition7(4.0f, -0.5f, 0.0f);


// SHADOW MAP
const uint32_t k_shadow_width = 2048;
const uint32_t k_shadow_height = 2048;
const float k_shadow_near = 1.0f;
const float k_shadow_far = 25.0f;

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
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return std::make_pair(fbo, depthMap);
}



void handleInput(const float deltaTime) {
  cameraController.handleInput(deltaTime);
}

void render(const Shader& shader_light, const Shader& shader_phong, const Shader& shader_depth, const Geometry& quad, const Geometry& cube, const Geometry& sphere, const Texture& texture_material, const Texture& texture_reflection, uint32_t fbo, uint32_t text_fbo) {
  glm::mat4 view = camera.getViewMatrix();
  const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), camera.getNear(), camera.getFar());

  // SHADOW MAP
  glm::vec3 lightDirWorld = normalize(dirLightDirection);
  glm::vec3 lightPosForShadow = -lightDirWorld * 10.0f; // Fake position for dir light
  glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, k_shadow_near, k_shadow_far);
  glm::mat4 lightView = glm::lookAt(lightPosForShadow, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 lightSpaceMatrix = lightProjection * lightView;

  // PASS 1: DEPTH MAP
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, k_shadow_width, k_shadow_height);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  
  shader_depth.use();
  shader_depth.set("lightSpaceMatrix", lightSpaceMatrix);
  
  // DEPTH PASS SCENE RENDER
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -1.0f ,0.0f));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::scale(model, glm::vec3(10.0f));
  shader_depth.set("model", model);
  quad.render();

  // Cube 0
  glm::mat4 model_cube0 = glm::mat4(1.0f);
  model_cube0 = glm::translate(model_cube0, cubePosition0);
  shader_depth.set("model", model_cube0);
  cube.render();

  // Cube 1
  glm::mat4 model_cube1 = glm::mat4(1.0f);
  model_cube1 = glm::translate(model_cube1, cubePosition1);
  shader_depth.set("model", model_cube1);
  cube.render();

  // Cube 2
  glm::mat4 model_cube2 = glm::mat4(1.0f);
  model_cube2 = glm::translate(model_cube2, cubePosition2);
  shader_depth.set("model", model_cube2);
  cube.render();

  // Cube 3
  glm::mat4 model_cube3 = glm::mat4(1.0f);
  model_cube3 = glm::translate(model_cube3, cubePosition3);
  shader_depth.set("model", model_cube3);
  cube.render();

  // Cube 4
  glm::mat4 model_cube4 = glm::mat4(1.0f);
  model_cube4 = glm::translate(model_cube4, cubePosition4);
  shader_depth.set("model", model_cube4);
  cube.render();

  // Cube 5
  glm::mat4 model_cube5 = glm::mat4(1.0f);
  model_cube5 = glm::translate(model_cube5, cubePosition5);
  shader_depth.set("model", model_cube5);
  cube.render();

  // Cube 6
  glm::mat4 model_cube6 = glm::mat4(1.0f);
  model_cube6 = glm::translate(model_cube6, cubePosition6);
  shader_depth.set("model", model_cube6);
  cube.render();

  // Cube 7
  glm::mat4 model_cube7 = glm::mat4(1.0f);
  model_cube7 = glm::translate(model_cube7, cubePosition7);
  shader_depth.set("model", model_cube7);
  cube.render();

  // PASS 2: RENDER
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  int fbWidth, fbHeight;
  glfwGetFramebufferSize(Window::instance()->getNativeWindow(), &fbWidth, &fbHeight);
  glViewport(0, 0, fbWidth, fbHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    glm::mat4 model_point_light_1 = glm::mat4(1.0f);
    model_point_light_1 = glm::translate(model_point_light_1, pointLightPosition1);
    model_point_light_1 = glm::scale(model_point_light_1, glm::vec3(0.1f));
    shader_light.set("model", model_point_light_1);
    shader_light.set("lightColor", pointLightColor1);
    sphere.render();

    // Spot lights
    glm::mat4 model_spot_light_0 = glm::mat4(1.0f);
    model_spot_light_0 = glm::translate(model_spot_light_0, spotLightPosition0);
    model_spot_light_0 = glm::scale(model_spot_light_0, glm::vec3(0.1f));
    shader_light.set("model", model_spot_light_0);
    shader_light.set("lightColor", spotLightColor0);
    sphere.render();

    glm::mat4 model_spot_light_1 = glm::mat4(1.0f);
    model_spot_light_1 = glm::translate(model_spot_light_1, spotLightPosition1);
    model_spot_light_1 = glm::scale(model_spot_light_1, glm::vec3(0.1f));
    shader_light.set("model", model_spot_light_1);
    shader_light.set("lightColor", spotLightColor1);
    sphere.render();
  }

  // ---------------------------------------
  {
    shader_phong.use();

    shader_phong.set("view", view);
    shader_phong.set("proj", proj);

    // SHADOW MAP
    shader_phong.set("lightSpaceMatrix", lightSpaceMatrix);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, text_fbo);
    shader_phong.set("depthMap", 2);

    texture_material.use(shader_phong, "material.diffuse", 0);
    texture_reflection.use(shader_phong, "material.specular", 1);
    shader_phong.set("material.shininess", 64);

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

    // Point Light 1

    shader_phong.set("pointLight1.position", glm::vec3(view * glm::vec4(pointLightPosition1, 1.0f)));

    shader_phong.set("pointLight1.ambient", pointLightColor1 * glm::vec3(0.1f));
    shader_phong.set("pointLight1.diffuse", pointLightColor1 * glm::vec3(0.8f));
    shader_phong.set("pointLight1.specular", glm::vec3(1.0f));
    shader_phong.set("pointLight1.constant", 1.0f);
    shader_phong.set("pointLight1.linear", 0.14f);
    shader_phong.set("pointLight1.quadratic", 0.07f);

    // Spot Light 0

    shader_phong.set("spotLight0.position", glm::vec3(view * glm::vec4(spotLightPosition0, 1.0f)));

    shader_phong.set("spotLight0.ambient", spotLightColor0 * glm::vec3(0.1f));
    shader_phong.set("spotLight0.diffuse", spotLightColor0 * glm::vec3(0.8f));
    shader_phong.set("spotLight0.specular", glm::vec3(1.0f));
    shader_phong.set("spotLight0.constant", 1.0f);
    shader_phong.set("spotLight0.linear", 0.14f);
    shader_phong.set("spotLight0.quadratic", 0.07f);

    shader_phong.set("spotLight0.direction", glm::vec3(view * glm::vec4(spotLightDirection0, 0.0f)));

    // SOFT CUT OFF
    shader_phong.set("spotLight0.cutOff", glm::cos(glm::radians(30.0f)));
    shader_phong.set("spotLight0.outerCutOff", glm::cos(glm::radians(35.0f)));

    // Spot Light 1

    shader_phong.set("spotLight1.position", glm::vec3(view * glm::vec4(spotLightPosition1, 1.0f)));

    shader_phong.set("spotLight1.ambient", spotLightColor1 * glm::vec3(0.1f));
    shader_phong.set("spotLight1.diffuse", spotLightColor1 * glm::vec3(0.8f));
    shader_phong.set("spotLight1.specular", glm::vec3(1.0f));
    shader_phong.set("spotLight1.constant", 1.0f);
    shader_phong.set("spotLight1.linear", 0.14f);
    shader_phong.set("spotLight1.quadratic", 0.07f);

    shader_phong.set("spotLight1.direction", glm::vec3(view * glm::vec4(spotLightDirection1, 0.0f)));

    // HARD CUT OFF
    shader_phong.set("spotLight1.cutOff", glm::cos(glm::radians(30.0f)));
    shader_phong.set("spotLight1.outerCutOff", glm::cos(glm::radians(30.0f)));

    // Spot Light 2

    // CAMERA
    shader_phong.set("spotLight2.position", glm::vec3(view * glm::vec4(camera.getPosition(), 1.0f)));
    shader_phong.set("spotLight2.direction", glm::vec3(view * glm::vec4(camera.getDirection(), 0.0f)));

    glm::vec3 activeFlashlightColor = Window::instance()->hasFlag(GLFW_KEY_ENTER) ? flashlightColor : glm::vec3(0.0f);

    shader_phong.set("spotLight2.ambient", activeFlashlightColor * glm::vec3(0.2f));
    shader_phong.set("spotLight2.diffuse", activeFlashlightColor * glm::vec3(2.0f));

    // TOGGLE FLASHLIGHT ON ENTER
    shader_phong.set("spotLight2.specular", Window::instance()->hasFlag(GLFW_KEY_ENTER) ? glm::vec3(1.5f) : glm::vec3(0.0f));
    shader_phong.set("spotLight2.constant", 1.0f);
    shader_phong.set("spotLight2.linear", 0.045f);
    shader_phong.set("spotLight2.quadratic", 0.016f);

    // SOFT CUT OFF
    shader_phong.set("spotLight2.cutOff", glm::cos(glm::radians(12.5f)));
    shader_phong.set("spotLight2.outerCutOff", glm::cos(glm::radians(17.5f)));

    // PHONG PASS SCENE RENDER
    glm::mat4 model_phong = glm::mat4(1.0f);
    model_phong = glm::translate(model_phong, glm::vec3(0.0f, -1.0f ,0.0f));
    model_phong = glm::rotate(model_phong, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model_phong = glm::scale(model_phong, glm::vec3(10.0f));
    shader_phong.set("model", model_phong);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model_phong))));
    quad.render();

    // Cube 0
    glm::mat4 model_cube0_phong = glm::mat4(1.0f);
    model_cube0_phong = glm::translate(model_cube0_phong, cubePosition0);
    shader_phong.set("model", model_cube0_phong);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model_cube0_phong))));
    cube.render();

    // Cube 1
    glm::mat4 model_cube1_phong = glm::mat4(1.0f);
    model_cube1_phong = glm::translate(model_cube1_phong, cubePosition1);
    shader_phong.set("model", model_cube1_phong);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model_cube1_phong))));
    cube.render();

    // Cube 2
    glm::mat4 model_cube2_phong = glm::mat4(1.0f);
    model_cube2_phong = glm::translate(model_cube2_phong, cubePosition2);
    shader_phong.set("model", model_cube2_phong);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model_cube2_phong))));
    cube.render();

    // Cube 3
    glm::mat4 model_cube3_phong = glm::mat4(1.0f);
    model_cube3_phong = glm::translate(model_cube3_phong, cubePosition3);
    shader_phong.set("model", model_cube3_phong);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model_cube3_phong))));
    cube.render();

    // Cube 4
    glm::mat4 model_cube4_phong = glm::mat4(1.0f);
    model_cube4_phong = glm::translate(model_cube4_phong, cubePosition4);
    shader_phong.set("model", model_cube4_phong);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model_cube4_phong))));
    cube.render();

    // Cube 5
    glm::mat4 model_cube5_phong = glm::mat4(1.0f);
    model_cube5_phong = glm::translate(model_cube5_phong, cubePosition5);
    shader_phong.set("model", model_cube5_phong);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model_cube5_phong))));
    cube.render();

    // Cube 6
    glm::mat4 model_cube6_phong = glm::mat4(1.0f);
    model_cube6_phong = glm::translate(model_cube6_phong, cubePosition6);
    shader_phong.set("model", model_cube6_phong);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model_cube6_phong))));
    cube.render();

    // Cube 7
    glm::mat4 model_cube7_phong = glm::mat4(1.0f);
    model_cube7_phong = glm::translate(model_cube7_phong, cubePosition7);
    shader_phong.set("model", model_cube7_phong);
    shader_phong.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * model_cube7_phong))));
    cube.render();
  }
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();
  window->setCaptureMouse(true);
  Input* input = Input::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Cube cube = Cube(1.0f);
  const Quad quad = Quad(1.0f);
  const Sphere sphere = Sphere(1.0f, 20, 20);
  const Texture texture_material(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
  const Texture texture_reflection(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
  const Shader shader_light(PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
  const Shader shader_phong(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
  const Shader shader_depth(PROJECT_PATH "depth.vert", PROJECT_PATH "depth.frag"); // SHADOW MAP
  auto fbo = createFBO(); // SHADOW MAP

  // CAMERA SENSITIVITY
  camera.setMouseSensitivity(0.2f);
  camera.setMovementSpeed(10.0f);

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
    render(shader_light, shader_phong, shader_depth, quad, cube, sphere, texture_material, texture_reflection, fbo.first, fbo.second);

    window->frame();
  }

  glDeleteFramebuffers(1, &fbo.first);
  glDeleteTextures(1, &fbo.second);
  return 0;
}
