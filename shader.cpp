#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(unsigned int index) noexcept : ID(index) {}
Shader::~Shader() noexcept = default;

Shader::Shader(Shader&& shader) noexcept {
    *this = std::move(shader);
}

Shader& Shader::operator=(Shader&& shader) noexcept
{
    if (this != &shader)
    {
        destroy();
        ID = shader.ID;
        shader.ID = 0;
    }

    return *this;
}

unsigned int Shader::getId() const {
    return ID;
}

// Активация шейдера
void Shader::use() const{
    glUseProgram(ID);
}

void Shader::destroy()
{
    if (ID) 
    {
        glDeleteProgram(ID);
        ID = 0;
    }
}

// Полезные uniform-функции
// ------------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------

void Shader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

// Полезные функции для проверки ошибок компиляции/связывания шейдеров
bool Shader::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return false;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return false;
        }
    }

    return true;
}

unsigned int load_shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    // 1. Получение исходного кода вершинного/фрагментного шейдера
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    // Убеждаемся, что объекты ifstream могут выбросить исключение:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // Открываем файлы
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        // Читаем содержимое файловых буферов
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // Закрываем файлы
        vShaderFile.close();
        fShaderFile.close();

        // Конвертируем в строковую переменную данные из потока
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // Если путь к геометрическому шейдеру присутствует, то также загружаем и геометрический шейдер
        if (geometryPath != nullptr)
        {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        return 0;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Компилируем шейдеры
    unsigned int vertex, fragment;

    // Вершинный шейдер
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    if (!Shader::checkCompileErrors(vertex, "VERTEX")) {
        return 0;
    }

    // Фрагментный шейдер
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    if (!Shader::checkCompileErrors(fragment, "FRAGMENT")) {
        return 0;
    }

    // Если был дан геометрический шейдер, то компилируем его
    unsigned int geometry;
    if (geometryPath != nullptr)
    {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        if (!Shader::checkCompileErrors(geometry, "GEOMETRY")) {
            return 0;
        }
    }

    // Шейдерная программа
    unsigned int index = glCreateProgram();
    glAttachShader(index, vertex);
    glAttachShader(index, fragment);
    if (geometryPath != nullptr) {
        glAttachShader(index, geometry);
    }
    glLinkProgram(index);

    if (!Shader::checkCompileErrors(index, "PROGRAM")) {
        return 0;
    }

    // После того, как мы связали шейдеры с нашей программой, удаляем их, т.к. они нам больше не нужны
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr) {
        glDeleteShader(geometry);
    }

    return index;
}