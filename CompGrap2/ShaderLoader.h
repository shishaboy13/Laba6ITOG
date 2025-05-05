#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <string>
#include <GL/glew.h>
#include "glm/glm.hpp"

class ShaderLoader {
public:
    ShaderLoader(const std::string& vertexPath, const std::string& fragmentPath);
    ~ShaderLoader();

    void Use() const;
    GLuint GetID() const;

    // ”прощенные методы передачи uniform-переменных
    void SetUniform1f(const std::string& name, float value);
    void SetUniform1i(const std::string& name, int value);
    void SetUniform3f(const std::string& name, float x, float y, float z);
    void SetUniform3f(const std::string& name, const glm::vec3& value);
    void SetUniformMatrix3fv(const std::string& name, const float* matrix);
    void SetUniformMatrix4fv(const std::string& name, const float* matrix);

private:
    GLuint programID;

    GLuint LoadShader(const std::string& path, GLenum shaderType);
    void CheckCompileErrors(GLuint shader, std::string type);
};

#endif