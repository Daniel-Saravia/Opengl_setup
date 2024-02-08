#include "glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::string readFile(const char* filePath);
unsigned int compileShader(unsigned int type, const std::string& source);
unsigned int createShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);

glm::vec3 sceneCenter = glm::vec3(0.0f, 0.0f, 0.0f);

// Camera settings
glm::vec3 cameraPositions[] = {
    glm::vec3(0.0f, 0.0f, 10.0f), // Front view
    glm::vec3(0.0f, 10.0f, 0.0f), // Top view
    glm::vec3(10.0f, 0.0f, 0.0f)  // Side view
};
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
int currentCameraPosition = 0; // Index of the current camera position

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Pyramid", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Load shaders from files
    std::string vertexShaderSource = readFile("vertex_shader.glsl");
    std::string fragmentShaderSource = readFile("fragment_shader.glsl");

    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    float vertices[] = {
        // Positions         // Colors
         0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // Top
        -0.5f, -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, // Front-left
         0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // Front-right
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, // Back-right
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f  // Back-left
    };
    unsigned int indices[] = {
        0, 1, 2, // Front face
        0, 2, 3, // Right face
        0, 3, 4, // Back face
        0, 4, 1, // Left face
        1, 2, 3, // Base right triangle
        1, 3, 4  // Base left triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    // Update the camera front vector based on the current camera position
    glm::vec3 target = cameraPositions[currentCameraPosition] + cameraFront;
    glm::mat4 view = glm::lookAt(cameraPositions[currentCameraPosition], target, cameraUp);
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    for (int i = 0; i < 3; ++i) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(i * 2.0f - 2.0f, 0.0f, 0.0f)); // Move pyramids along x-axis
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        currentCameraPosition = 0;
        cameraFront = glm::normalize(sceneCenter - cameraPositions[currentCameraPosition]);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        currentCameraPosition = 1;
        // For top view, ensure the camera looks towards the center; adjust cameraUp if necessary
        cameraFront = glm::normalize(sceneCenter - cameraPositions[currentCameraPosition]);
        cameraUp = glm::vec3(0.0f, 0.0f, -1.0f); // May need adjustment for top view
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        currentCameraPosition = 2;
        cameraFront = glm::normalize(sceneCenter - cameraPositions[currentCameraPosition]);
    }
}

std::string readFile(const char* filePath) {
    std::ifstream fileStream(filePath, std::ios::in);
    std::string content;

    if (!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::stringstream sstr;
    sstr << fileStream.rdbuf();
    content = sstr.str();
    fileStream.close();

    return content;
}

unsigned int compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!\n" << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int createShaderProgram(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

