#include "ShaderLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

ShaderLoader::ShaderLoader(const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint vertexShader = LoadShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = LoadShader(fragmentPath, GL_FRAGMENT_SHADER);

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    CheckCompileErrors(programID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

ShaderLoader::~ShaderLoader() {
    glDeleteProgram(programID);
}

void ShaderLoader::Use() const {
    glUseProgram(programID);
}

GLuint ShaderLoader::GetID() const {
    return programID;
}

GLuint ShaderLoader::LoadShader(const std::string& path, GLenum shaderType) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ќшибка: не удалось открыть файл " << path << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream shaderStream;
    shaderStream << file.rdbuf();
    file.close();
    std::string shaderCode = shaderStream.str();
    const char* shaderSource = shaderCode.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    CheckCompileErrors(shader, (shaderType == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT");

    return shader;
}

void ShaderLoader::CheckCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ќшибка компил€ции шейдера (" << type << "):\n" << infoLog << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ќшибка линковки программы:\n" << infoLog << std::endl;
        }
    }
}

void ShaderLoader::SetUniform1f(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderLoader::SetUniform1i(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderLoader::SetUniform3f(const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}

void ShaderLoader::SetUniform3f(const std::string& name, const glm::vec3& value) {
    glUniform3f(glGetUniformLocation(programID, name.c_str()), value.x, value.y, value.z);
}

void ShaderLoader::SetUniformMatrix3fv(const std::string& name, const float* matrix) {
    glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, matrix);
}

void ShaderLoader::SetUniformMatrix4fv(const std::string& name, const float* matrix) {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, matrix);
}