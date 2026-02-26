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

bool checkShader(uint32_t shader) {
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char log[512];
    glGetShaderInfoLog(shader, 512, nullptr, log);
    std::cout << "Error compiling shader " << shader << " - " << log << std::endl;
  }

  return success;
}

bool checkProgram(uint32_t program) {
  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char log[512];
    glGetProgramInfoLog(program, 512, nullptr, log);
    std::cout << "Error linking program " << program << " - " << log << std::endl;
  }

  return success;
}

uint32_t createProgram() {
  const char* vertexShaderSource = "#version 330 core\n"
    "layout (location=0) in vec3 aPos;\n"
    "void main() {\n"
    "  gl_Position = vec4(aPos, 1.0);\n"
    "}\0";

  const uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);
  checkShader(vertexShader);

  const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "  FragColor = vec4(0.7, 0.2, 0.2, 1.0);\n"
    "}\0";

  const uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);
  checkShader(fragmentShader);

  const uint32_t program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  checkProgram(program);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return program;
}

std::tuple<uint32_t, uint32_t, uint32_t> createVertexData() {
  float vertices[] = { 0.5f, 0.5f, 0.0f,   // top right
                       0.5f, -0.5f, 0.0f,  // bottom right
                      -0.5f, -0.5f, 0.0f,  // bottom left
                      -0.5f, 0.5f, 0.0f    // top left
  };

  uint32_t indices[] = { 0, 3, 1,  // first triangle
                         1, 3, 2   // second triangle
  };

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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);  // Unbind VAO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return std::make_tuple(VAO, VBO, EBO);
}

void render(const uint32_t program, const uint32_t VAO) {
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(program);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

int main(int argc, char* argv[]) {
  if (!glfwInit()) {
    std::cout << "Error Initializing GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

  auto vertexData = createVertexData();
  uint32_t program = createProgram();

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  while (!glfwWindowShouldClose(window)) {
    handleInput(window);
    //update();
    render(program, std::get<0>(vertexData));

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &std::get<0>(vertexData));
  glDeleteBuffers(1, &std::get<1>(vertexData));  
  glDeleteBuffers(1, &std::get<2>(vertexData));
  glDeleteProgram(program);

  glfwTerminate();

  return 0;
}