#include "engine/shader.hpp"

#pragma warning(push, 0)
#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const char* vertexPath, const char* fragmentPath,
    const char* geometryPath) {

    std::string sVertexCode, sFragmentCode, sGeometryCode;
    loadShader(vertexPath, &sVertexCode);
    loadShader(fragmentPath, &sFragmentCode);
    if (geometryPath) {
        loadShader(geometryPath, &sGeometryCode);
    }

    const char* vertexCode = sVertexCode.c_str();
    const char* fragmentCode = sFragmentCode.c_str();

    const uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, nullptr);
    glCompileShader(vertex);
    checkErrors(vertex, Type::Vertex);

    const uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, nullptr);
    glCompileShader(fragment);
    checkErrors(fragment, Type::Fragment);

    uint32_t geometry = 0;
    if (geometryPath) {
        const char* geometryCode = sGeometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometryCode, nullptr);
        glCompileShader(geometry);
        checkErrors(geometry, Type::Geometry);
    }

    _id = glCreateProgram();
    glAttachShader(_id, vertex);
    glAttachShader(_id, fragment);
    if (geometryPath) {
        glAttachShader(_id, geometry);
    }
    glLinkProgram(_id);
    checkErrors(_id, Type::Program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath) {
        glDeleteShader(geometry);
    }
}

Shader::~Shader() {
    glDeleteProgram(_id);
}

void Shader::use() const {
    glUseProgram(_id);
}

void Shader::set(const char* name, const bool value) const {
    glUniform1i(glGetUniformLocation(_id, name), static_cast<int>(value));
}

void Shader::set(const char* name, const int value) const {
    glUniform1i(glGetUniformLocation(_id, name), value);
}

void Shader::set(const char* name, const float value) const {
    glUniform1f(glGetUniformLocation(_id, name), value);
}

void Shader::set(const char* name, const float value1, const float value2) const {
    glUniform2f(glGetUniformLocation(_id, name), value1, value2);
}

void Shader::set(const char* name, const float value1, const float value2,
    const float value3) const {
    glUniform3f(glGetUniformLocation(_id, name), value1, value2, value3);
}

void Shader::set(const char* name, const float value1, const float value2,
    const float value3, const float value4) const {
    glUniform4f(glGetUniformLocation(_id, name), value1, value2, value3, value4);
}

void Shader::set(const char* name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(_id, name), 1, glm::value_ptr(value));
}

void Shader::set(const char* name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(_id, name), 1, glm::value_ptr(value));
}

void Shader::set(const char* name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(_id, name), 1, glm::value_ptr(value));
}

void Shader::set(const char* name, const glm::mat2& value) const {
    glUniformMatrix2fv(glGetUniformLocation(_id, name), 1, GL_FALSE,
        glm::value_ptr(value));
}

void Shader::set(const char* name, const glm::mat3& value) const {
    glUniformMatrix3fv(glGetUniformLocation(_id, name), 1, GL_FALSE,
        glm::value_ptr(value));
}

void Shader::set(const char* name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(_id, name), 1, GL_FALSE,
        glm::value_ptr(value));
}

void Shader::checkErrors(const uint32_t shader, const Type type) {
    int success;
    char log[512];
    if (type != Type::Program) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, nullptr, log);
            std::cout << "Error Compiling Shader " <<
                (type == Type::Vertex ? "Vertex " : "Fragment ") << log << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 512, nullptr, log);
            std::cout << "Error Linking Program " << log << std::endl;
        }
    }
}

void Shader::loadShader(const char* path, std::string* code) {
    std::ifstream file;

    file.open(path, std::ios_base::in);
    if (file) {
        std::stringstream stream;
        stream << file.rdbuf();
        *code = stream.str();
    } else {
        std::cout << "Error Loading Shader " << path << std::endl;
    }
}