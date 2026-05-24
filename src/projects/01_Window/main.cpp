#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void onChangeFrameBufferSize(GLFWwindow* window, const int32_t width, const int32_t height) {
  glViewport(0, 0, width, height);
  std::cout << width << " " << height << std::endl;
}

void handleInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void render() {
  glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char* argv[]) {
  if (!glfwInit()) {
    std::cout << "Error Initializing GLFW" << std::endl;
    return -1;
  }

#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "60GIIN", nullptr, nullptr);
  if (!window) {
    std::cout << "Error Creating Window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGL()) {
    std::cout << "Error Initializing GLAD" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, onChangeFrameBufferSize);

  glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

  while (!glfwWindowShouldClose(window)) {
    handleInput(window);
    //update();
    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
