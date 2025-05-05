#define GLEW_DLL
#define GLFW_DLL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ShaderLoader.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Model.h"
#include <iostream>

// Параметры камеры
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Углы Эйлера
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
bool firstMouse = true;
float sensitivity = 0.1f;
float cameraSpeed = 0.05f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: GLFW initialization failed\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Petrov Kirill", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: Window creation failed\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glEnable(GL_DEPTH_TEST);

    ShaderLoader shader("vertex_shader.glsl", "fragment_shader.glsl");
    Model ourModel("model.obj"); // Убедитесь, что модель содержит нормали

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        // Матрицы преобразования
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

        // Нормальная матрица
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader.SetUniformMatrix3fv("normalMatrix", glm::value_ptr(normalMatrix));

        // Передача матриц в шейдер
        shader.SetUniformMatrix4fv("projection", glm::value_ptr(projection));
        shader.SetUniformMatrix4fv("view", glm::value_ptr(view));
        shader.SetUniformMatrix4fv("model", glm::value_ptr(model));

        // Позиция камеры для расчета specular
        shader.SetUniform3f("viewPos", cameraPos);

        // Настройка материала
        glm::vec3 matAmbient(1.0f, 0.5f, 0.31f);
        glm::vec3 matDiffuse(1.0f, 0.5f, 0.31f);
        glm::vec3 matSpecular(0.5f, 0.5f, 0.5f);

        shader.SetUniform3f("material.ambient", matAmbient);
        shader.SetUniform3f("material.diffuse", matDiffuse);
        shader.SetUniform3f("material.specular", matSpecular);
        shader.SetUniform1f("material.shininess", 32.0f);

        // Настройка света
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        glm::vec3 lightAmbient = lightColor * glm::vec3(0.2f);
        glm::vec3 lightDiffuse = lightColor * glm::vec3(0.8f);
        glm::vec3 lightSpecular = lightColor;
        glm::vec3 lightPos(-3.2f, 4.0f, 4.0f);

        shader.SetUniform3f("light_1.ambient", lightAmbient);
        shader.SetUniform3f("light_1.diffuse", lightDiffuse);
        shader.SetUniform3f("light_1.specular", lightSpecular);
        shader.SetUniform3f("light_1.Position", lightPos);;

        // Отрисовка модели
        ourModel.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}