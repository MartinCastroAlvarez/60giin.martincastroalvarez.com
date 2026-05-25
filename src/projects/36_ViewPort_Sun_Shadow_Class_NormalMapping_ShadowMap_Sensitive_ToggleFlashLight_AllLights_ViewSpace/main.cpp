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
#include "engine/shadow.hpp"
#include "engine/viewport.hpp"
#include "engine/geometry/cube.hpp"
#include "engine/geometry/pyramid.hpp"
#include "engine/geometry/quad.hpp"
#include "engine/geometry/sphere.hpp"
#include "engine/oscillation.hpp"

// Camera parameters
Camera camera(glm::vec3(0.0f, 5.0f, 10.0f));
CameraController cameraController(camera);

// DIRECTIONAL LIGHT CONFIGURATION
glm::vec3 dirLightDirection(-0.4f, -1.0f, -0.5f); // Direction of the sun / shadow
glm::vec3 dirLightColor(1.0f, 1.0f, 1.0f);

// NORMAL MAPPING CONFIGURATION
float normalIntensity = 1.0f;     // How strong the bumps appear (0.0 = completely flat, 1.0 = normal)

// SUN CONFIGURATION
float sunSpeed = 1.0f;
float sunRadius = 8.0f;

// SMALL CAMERA CONFIGURATION
glm::vec3 smallCamPos(0.0f, 15.0f, 0.0f);
glm::vec3 smallCamTarget(0.0f, 0.0f, 0.0f);
glm::vec3 smallCamUp(0.0f, 0.0f, -1.0f);
int smallCamWidth = 300;
int smallCamHeight = 200;
int smallCamX = 20;
int smallCamY = 20;

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


void handleInput(const float deltaTime) {
  cameraController.handleInput(deltaTime);
}

void drawQuad(const Shader& shader, const Quad& quad, const glm::mat4& view = glm::mat4(1.0f)) {
  glm::mat4 m = glm::mat4(1.0f);
  m = glm::translate(m, glm::vec3(0.0f, -1.0f ,0.0f));
  m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  m = glm::scale(m, glm::vec3(10.0f));
  shader.set("model", m);
  shader.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * m))));
  quad.render();
}

void drawCube(const Shader& shader, const Cube& cube, const glm::vec3& pos, const glm::mat4& view = glm::mat4(1.0f)) {
  glm::mat4 m = glm::mat4(1.0f);
  m = glm::translate(m, pos);
  shader.set("model", m);
  shader.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * m))));
  cube.render();
}

void drawPyramid(const Shader& shader, const Pyramid& pyramid, const glm::vec3& pos, const glm::mat4& view = glm::mat4(1.0f)) {
  glm::mat4 m = glm::mat4(1.0f);
  m = glm::translate(m, pos);
  shader.set("model", m);
  shader.set("normalMat", glm::transpose(glm::inverse(glm::mat3(view * m))));
  pyramid.render();
}

void drawPointLight(const Shader& shader_light, const Sphere& sphere, const glm::vec3& pos, const glm::vec3& color, float scale = 0.1f) {
  glm::mat4 m = glm::mat4(1.0f);
  m = glm::translate(m, pos);
  m = glm::scale(m, glm::vec3(scale));
  shader_light.set("model", m);
  shader_light.set("lightColor", color);
  sphere.render();
}

void setupPointLight(const Shader& shader_phong, const std::string& name, const glm::vec3& pos, const glm::vec3& color, const glm::mat4& view, float linear = 0.14f, float quadratic = 0.07f) {
  shader_phong.set((name + ".position").c_str(), glm::vec3(view * glm::vec4(pos, 1.0f)));
  shader_phong.set((name + ".ambient").c_str(), color * glm::vec3(0.1f));
  shader_phong.set((name + ".diffuse").c_str(), color * glm::vec3(0.8f));
  shader_phong.set((name + ".specular").c_str(), glm::vec3(1.0f));
  shader_phong.set((name + ".constant").c_str(), 1.0f);
  shader_phong.set((name + ".linear").c_str(), linear);
  shader_phong.set((name + ".quadratic").c_str(), quadratic);
}

void setupSpotLight(const Shader& shader_phong, const std::string& name, const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& color, const glm::mat4& view, float cutOff = 30.0f, float outerCutOff = 35.0f, float diffuse = 0.8f, float specular = 1.0f, float linear = 0.14f, float quadratic = 0.07f, float ambientMult = 0.1f) {
  shader_phong.set((name + ".position").c_str(), glm::vec3(view * glm::vec4(pos, 1.0f)));
  shader_phong.set((name + ".direction").c_str(), glm::vec3(view * glm::vec4(dir, 0.0f)));
  shader_phong.set((name + ".ambient").c_str(), color * glm::vec3(ambientMult));
  shader_phong.set((name + ".diffuse").c_str(), color * glm::vec3(diffuse));
  shader_phong.set((name + ".specular").c_str(), glm::vec3(specular));
  shader_phong.set((name + ".constant").c_str(), 1.0f);
  shader_phong.set((name + ".linear").c_str(), linear);
  shader_phong.set((name + ".quadratic").c_str(), quadratic);
  shader_phong.set((name + ".cutOff").c_str(), glm::cos(glm::radians(cutOff)));
  shader_phong.set((name + ".outerCutOff").c_str(), glm::cos(glm::radians(outerCutOff)));
}

void render(const Shader& shader_light, const Shader& shader_phong, const Shader& shader_depth, const Shader& shader_fbo, const Geometry& quad, const Geometry& cube, const Geometry& pyramid, const Geometry& sphere, const Texture& texture_material, const Texture& texture_reflection, const Texture& texture_normal, float time) {
  Oscillation sunOscillation(glm::vec3(0.0f, 5.0f, 0.0f), sunRadius, glm::vec3(sunSpeed, 0.0f, sunSpeed));
  glm::vec3 sunPosition = sunOscillation.getPosition(time);

  glm::mat4 view = camera.getViewMatrix();
  const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)Window::instance()->getWidth() / (float)Window::instance()->getHeight(), camera.getNear(), camera.getFar());

  // SHADOW MAP
  glm::vec3 lightDirWorld = normalize(-sunPosition); // Point shadow map camera from sun to origin
  Shadow* shadow = Shadow::instance();
  // Ensure we place the camera at distance 15.0 to encompass the sun's max orbit distance
  shadow->setDistance(15.0f);
  glm::mat4 lightSpaceMatrix = shadow->getLightSpaceMatrix(lightDirWorld);

  // PASS 1: DEPTH MAP
  shadow->bindFBO();
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_FRONT);

  shader_depth.use();
  shader_depth.set("lightSpaceMatrix", lightSpaceMatrix);

  // DEPTH PASS SCENE RENDER
  drawQuad(shader_depth, static_cast<const Quad&>(quad));
  drawCube(shader_depth, static_cast<const Cube&>(cube), cubePosition0);
  drawCube(shader_depth, static_cast<const Cube&>(cube), cubePosition1);
  drawPyramid(shader_depth, static_cast<const Pyramid&>(pyramid), cubePosition2);
  drawPyramid(shader_depth, static_cast<const Pyramid&>(pyramid), cubePosition3);
  drawCube(shader_depth, static_cast<const Cube&>(cube), cubePosition4);
  drawPyramid(shader_depth, static_cast<const Pyramid&>(pyramid), cubePosition5);
  drawPyramid(shader_depth, static_cast<const Pyramid&>(pyramid), cubePosition6);
  drawCube(shader_depth, static_cast<const Cube&>(cube), cubePosition7);

  // PASS 2: RENDER
  int fbWidth, fbHeight;
  glfwGetFramebufferSize(Window::instance()->getNativeWindow(), &fbWidth, &fbHeight);
  shadow->unbindFBO(fbWidth, fbHeight);
  glCullFace(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto draw = [&](const glm::mat4& sceneView, const glm::mat4& sceneProj) {

  {
    shader_light.use();

    shader_light.set("view", sceneView);
    shader_light.set("proj", sceneProj);

    // Point lights
    drawPointLight(shader_light, static_cast<const Sphere&>(sphere), pointLightPosition0, pointLightColor0);
    drawPointLight(shader_light, static_cast<const Sphere&>(sphere), pointLightPosition1, pointLightColor1);
    // Sun (Point Light 2)
    drawPointLight(shader_light, static_cast<const Sphere&>(sphere), sunPosition, glm::vec3(1.0f), 0.2f);
    // Spot lights
    drawPointLight(shader_light, static_cast<const Sphere&>(sphere), spotLightPosition0, spotLightColor0);
    drawPointLight(shader_light, static_cast<const Sphere&>(sphere), spotLightPosition1, spotLightColor1);
  }

  // ---------------------------------------
  {
    shader_phong.use();

    shader_phong.set("view", sceneView);
    shader_phong.set("proj", sceneProj);

    // SHADOW MAP
    shader_phong.set("lightSpaceMatrix", lightSpaceMatrix);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, shadow->getDepthMap());
    shader_phong.set("depthMap", 2);

    texture_material.use(shader_phong, "material.diffuse", 0);
    texture_reflection.use(shader_phong, "material.specular", 1);
    // Unit 2 is used by the depth map
    texture_normal.use(shader_phong, "material.normal", 3); // NORMAL MAPPING
    shader_phong.set("normalIntensity", normalIntensity);   // NORMAL MAPPING
    shader_phong.set("material.shininess", 64);

    // Directional Light ---------------------------------------

    shader_phong.set("dirLight.direction", glm::vec3(sceneView * glm::vec4(lightDirWorld, 0.0f)));
    shader_phong.set("dirLight.ambient", dirLightColor * glm::vec3(0.1f));
    shader_phong.set("dirLight.diffuse", dirLightColor * glm::vec3(0.8f));
    shader_phong.set("dirLight.specular", glm::vec3(1.0f));
    shader_phong.set("shadowIntensity", shadow->getIntensity());

    // Point Light 0
    setupPointLight(shader_phong, "pointLight0", pointLightPosition0, pointLightColor0, sceneView);
    // Point Light 1
    setupPointLight(shader_phong, "pointLight1", pointLightPosition1, pointLightColor1, sceneView);
    // Sun (Point Light 2) - Disabled because it washes out the shadows from dirLight!
    // setupPointLight(shader_phong, "pointLight2", sunPosition, glm::vec3(1.0f), sceneView, 0.09f, 0.032f);

    // Spot Light 0
    setupSpotLight(shader_phong, "spotLight0", spotLightPosition0, spotLightDirection0, spotLightColor0, sceneView);
    // Spot Light 1
    setupSpotLight(shader_phong, "spotLight1", spotLightPosition1, spotLightDirection1, spotLightColor1, sceneView, 30.0f, 30.0f);

    // Spot Light 2 (CAMERA FLASHLIGHT)
    glm::vec3 activeFlashlightColor = Window::instance()->hasFlag(GLFW_KEY_ENTER) ? flashlightColor : glm::vec3(0.0f);
    setupSpotLight(shader_phong, "spotLight2", camera.getPosition(), camera.getDirection(), activeFlashlightColor, sceneView, 12.5f, 17.5f, 2.0f, Window::instance()->hasFlag(GLFW_KEY_ENTER) ? 1.5f : 0.0f, 0.045f, 0.016f, 0.2f);

    // PHONG PASS SCENE RENDER
    drawQuad(shader_phong, static_cast<const Quad&>(quad), sceneView);
    drawCube(shader_phong, static_cast<const Cube&>(cube), cubePosition0, sceneView);
    drawCube(shader_phong, static_cast<const Cube&>(cube), cubePosition1, sceneView);
    drawPyramid(shader_phong, static_cast<const Pyramid&>(pyramid), cubePosition2, sceneView);
    drawPyramid(shader_phong, static_cast<const Pyramid&>(pyramid), cubePosition3, sceneView);
    drawCube(shader_phong, static_cast<const Cube&>(cube), cubePosition4, sceneView);
    drawPyramid(shader_phong, static_cast<const Pyramid&>(pyramid), cubePosition5, sceneView);
    drawPyramid(shader_phong, static_cast<const Pyramid&>(pyramid), cubePosition6, sceneView);
    drawCube(shader_phong, static_cast<const Cube&>(cube), cubePosition7, sceneView);
  }
  };

  // PASS 2: Main Camera
  glfwGetFramebufferSize(Window::instance()->getNativeWindow(), &fbWidth, &fbHeight);
  glViewport(0, 0, fbWidth, fbHeight);
  glDisable(GL_SCISSOR_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 mainView = camera.getViewMatrix();
  glm::mat4 mainProj = glm::perspective(glm::radians(camera.getFOV()), (float)fbWidth / (float)fbHeight, camera.getNear(), camera.getFar());
  draw(mainView, mainProj);

  // PASS 3: Small Camera (Render to FBO)
  Viewport* viewport = Viewport::instance();
  viewport->bindFBO();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 smallView = glm::lookAt(smallCamPos, smallCamTarget, smallCamUp);
  glm::mat4 smallProj = glm::perspective(glm::radians(45.0f), (float)smallCamWidth / (float)smallCamHeight, 0.1f, 100.0f);
  draw(smallView, smallProj);
  viewport->unbindFBO();

  // PASS 4: Draw FBO to screen
  glDisable(GL_DEPTH_TEST);
  glViewport(smallCamX, smallCamY, smallCamWidth, smallCamHeight);
  
  shader_fbo.use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, viewport->getTexture());
  shader_fbo.set("screen_text", 0);
  quad.render();

  glEnable(GL_DEPTH_TEST);
}

int main(int argc, char* argv[]) {
  Window* window = Window::instance();
  window->setCaptureMouse(true);
  Input* input = Input::instance();

  stbi_set_flip_vertically_on_load(true);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  const Cube cube = Cube(1.0f);
  const Pyramid pyramid = Pyramid(1.5f, 1.0f);
  const Quad quad = Quad(1.0f);
  const Sphere sphere = Sphere(1.0f, 20, 20);
  const Texture texture_material(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
  const Texture texture_reflection(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
  const Texture texture_normal(ASSETS_PATH "textures/bricks_normal.png", Texture::Format::RGB); // NORMAL MAPPING
  const Shader shader_light(PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
  const Shader shader_phong(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
  const Shader shader_depth(PROJECT_PATH "depth.vert", PROJECT_PATH "depth.frag"); // SHADOW MAP
  const Shader shader_fbo(PROJECT_PATH "fbo.vert", PROJECT_PATH "fbo.frag");  // SMALL WINDOW

  Viewport* viewport = Viewport::instance();
  viewport->init(smallCamWidth, smallCamHeight);

  // SHADOW MAP
  Shadow* shadow = Shadow::instance();
  shadow->init();
  shadow->setOrthoBoxSize(10.0f);
  shadow->setDistance(15.0f); // Make sure it encompasses the oscillating sun
  shadow->setIntensity(0.8f);

  // CAMERA SENSITIVITY
  camera.setMouseSensitivity(0.2f);
  camera.setMovementSpeed(10.0f);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Camera initial direction
  camera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

  float lastFrame = 0.0f;

  while (window->isAlive()) {
    const auto currentFrame = glfwGetTime();
    const float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    handleInput(deltaTime); // Keyboard enabled!

    // update();
    render(
      shader_light,
      shader_phong,
      shader_depth,
      shader_fbo,
      quad,
      cube,
      pyramid,
      sphere,
      texture_material,
      texture_reflection,
      texture_normal,
      currentFrame
    );

    window->frame();
  }


  return 0;
}
