#pragma once

#pragma warning(push, 0)
#include <glm/glm.hpp>
#pragma warning(pop)

#include <string>

class Shader {
    public:

        enum class Type {
            Vertex = 0,
            Fragment = 1,
            Geometry = 2,
            Program = 3,
        };

        Shader() = delete;
        Shader(const char* vertexPath, const char* fragmentPath,
            const char* geometryPath = nullptr);
        ~Shader();

        Shader(const Shader&) = default;
        Shader(Shader&&) = default;
        Shader& operator=(const Shader&) = default;
        Shader& operator=(Shader&&) = default;

        void use() const;

        void set(const char* name, bool value) const;
        void set(const char* name, int value) const;
        void set(const char* name, float value) const;
        void set(const char* name, float value1, float value2) const;
        void set(const char* name, float value1, float value2, float value3) const;
        void set(const char* name, float value1, float value2, float value3, float value4) const;
        void set(const char* name, const glm::vec2& value) const;
        void set(const char* name, const glm::vec3& value) const;
        void set(const char* name, const glm::vec4& value) const;
        void set(const char* name, const glm::mat2& value) const;
        void set(const char* name, const glm::mat3& value) const;
        void set(const char* name, const glm::mat4& value) const;
    private:
        static void checkErrors(uint32_t shader, Type type);
        static void loadShader(const char* path, std::string* code);

        uint32_t _id;
};