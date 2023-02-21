#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class Shader
{
public:
    explicit Shader(unsigned int index = 0) noexcept;
    Shader(const Shader& shader) = delete;
    Shader(Shader&& shader) noexcept;
    ~Shader() noexcept;

    Shader& operator=(const Shader& shader) = delete;
    Shader& operator=(Shader&& shader) noexcept;
    // Активация шейдера
    void use() const;
    // Kill shader
    void destroy();

    // Полезные uniform-функции
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;

    unsigned int getId() const;

    static bool checkCompileErrors(GLuint shader, std::string type);

private:
    unsigned int ID;
};

unsigned int load_shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);