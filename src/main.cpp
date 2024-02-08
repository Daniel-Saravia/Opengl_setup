// Include the necessary headers for OpenGL functionality, window management, and math operations.
#include "glad.h"                       // GLAD manages function pointers for OpenGL so we can use all the OpenGL functions.
#include <GLFW/glfw3.h>                 // GLFW provides a simple API for creating windows, contexts and managing input.
#include <glm/glm.hpp>                  // GLM is a mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
#include <glm/gtc/matrix_transform.hpp> // Provides functions for generating common transformation matrices.
#include <glm/gtc/type_ptr.hpp>         // Provides functions to convert GLM types to plain C++ types.
#include <iostream>                     // Included for input/output operations.
#include <fstream>                      // File stream, used for reading shader files.
#include <sstream>                      // String stream, used for buffering string data read from files.
#include <string>                       // Used for string operations.

// Function declarations. These functions will be defined later in the code.
void framebuffer_size_callback(GLFWwindow *window, int width, int height);                            // Callback function for when the window size changes.
void processInput(GLFWwindow *window);                                                                // Processes input from the user.
std::string readFile(const char *filePath);                                                           // Reads the content of a file and returns it as a string.
unsigned int compileShader(unsigned int type, const std::string &source);                             // Compiles a shader from source code.
unsigned int createShaderProgram(const std::string &vertexShader, const std::string &fragmentShader); // Links vertex and fragment shaders into a shader program.

// Scene settings
glm::vec3 sceneCenter = glm::vec3(0.0f, 0.0f, 0.0f); // Center of the scene, used for camera orientation.

// Camera settings
glm::vec3 cameraPositions[] = {
    glm::vec3(0.0f, 0.0f, 10.0f), // Front view position of the camera.
    glm::vec3(0.0f, 10.0f, 0.0f), // Top view position of the camera.
    glm::vec3(10.0f, 0.0f, 0.0f)  // Side view position of the camera.
};
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // The initial forward direction of the camera.
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);     // The up direction of the camera, used to define the "up" in the world space.
int currentCameraPosition = 0;                        // Index to track the current camera position from the cameraPositions array.

int main()
{
    // Initialize GLFW library
    glfwInit();

    // Set GLFW to use OpenGL version 3.3, ensuring forward compatibility and core profile features
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Additional hint for macOS compatibility: Enables features from newer OpenGL versions
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Pyramid", nullptr, nullptr);
    if (window == nullptr) // Check if the GLFW window failed to create
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // Terminate GLFW, freeing any resources allocated by GLFW.
        return -1;       // Return -1 indicating the program failed to run properly
    }
    glfwMakeContextCurrent(window); // Make the window's context current

    // Set the function to be called when the window size is changed
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD before calling any OpenGL function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1; // Return -1 indicating the program failed to run properly
    }

    // Load shaders from files, compile them, and link them into a shader program
    std::string vertexShaderSource = readFile("vertex_shader.glsl");
    std::string fragmentShaderSource = readFile("fragment_shader.glsl");
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Define the vertices of our pyramid, including position and color data
    float vertices[] = {
        // Positions          // Colors
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // Top vertex
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // Front-left vertex
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // Front-right vertex
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, // Back-right vertex
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f // Back-left vertex
    };
    // Define the indices for the pyramid, telling OpenGL which vertices make up each triangle
    unsigned int indices[] = {
        0, 1, 2, // Front face triangle
        0, 2, 3, // Right face triangle
        0, 3, 4, // Back face triangle
        0, 4, 1, // Left face triangle
        1, 2, 3, // Base right triangle
        1, 3, 4  // Base left triangle
    };

    // Generate and bind the Vertex Array Object (VAO), Vertex Buffer Object (VBO), and Element Buffer Object (EBO)
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO); // Generates one Vertex Array Object
    glGenBuffers(1, &VBO);      // Generates one Vertex Buffer Object
    glGenBuffers(1, &EBO);      // Generates one Element Buffer Object

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attribute(s).
    glBindVertexArray(VAO);

    // Copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Copy our index array in a buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);                   // Position attribute
    glEnableVertexAttribArray(0);                                                                    // Enable the position attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))); // Color attribute
    glEnableVertexAttribArray(1);                                                                    // Enable the color attribute

    // The render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window); // Check if the user has triggered any input (like pressing the ESC key)

        // Clear the screen to a dark green color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        // Calculate the view matrix using the camera position, target direction, and up vector
        glm::vec3 target = cameraPositions[currentCameraPosition] + cameraFront;
        glm::mat4 view = glm::lookAt(cameraPositions[currentCameraPosition], target, cameraUp);
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Calculate the projection matrix for a perspective view
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw the pyramids
        for (int i = 0; i < 3; ++i) // Iterate through each pyramid
        {
            // Calculate the model matrix for each pyramid and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);                                     // Start with the identity matrix
            model = glm::translate(model, glm::vec3(i * 2.0f - 2.0f, 0.0f, 0.0f)); // Move pyramids along x-axis
            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glBindVertexArray(VAO);                               // Bind the VAO (it was already bound, but doing so in case it changed)
            glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0); // Draw the pyramid
        }

        glfwSwapBuffers(window); // Swap the front and back buffers
        glfwPollEvents();        // Poll for and process events
    }

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate(); // Clean all the GLFW resources.
    return 0;
}

// Callback function to adjust the viewport when the window size is changed.
// This is necessary to ensure that the viewport matches the new window size whenever it is resized,
// preventing the rendered scene from being distorted.
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // Sets the size of the rendering viewport. This should match the new window size.
    // Parameters are the lower left corner (x, y) followed by width and height.
    glViewport(0, 0, width, height);
}

// Function to process user input. It checks for specific key presses and reacts accordingly.
void processInput(GLFWwindow *window)
{
    // Checks if the ESC key was pressed. If so, the window should close.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); // This function sets the condition to close the GLFW window.

    // Adjusts the camera position based on number key presses. This allows the user
    // to switch between predefined camera views dynamically during runtime.

    // Checks if the '1' key is pressed to set the camera to the front view.
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        currentCameraPosition = 0;                                                          // Sets the index for the front view position from the cameraPositions array.
        cameraFront = glm::normalize(sceneCenter - cameraPositions[currentCameraPosition]); // Recalculates the camera's front vector.
    }

    // Checks if the '2' key is pressed to set the camera to the top view.
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        currentCameraPosition = 1;                                                          // Sets the index for the top view position.
        cameraFront = glm::normalize(sceneCenter - cameraPositions[currentCameraPosition]); // Recalculates the camera's front vector.
        cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);                                            // Adjusts the camera's up vector for the top view to maintain the correct orientation.
    }

    // Checks if the '3' key is pressed to set the camera to the side view.
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        currentCameraPosition = 2;                                                          // Sets the index for the side view position.
        cameraFront = glm::normalize(sceneCenter - cameraPositions[currentCameraPosition]); // Recalculates the camera's front vector.
    }
}

// Function to read shader source code from a file.
// filePath: Path to the shader file.
std::string readFile(const char *filePath)
{
    // Create an input file stream for reading the file.
    std::ifstream fileStream(filePath, std::ios::in);
    std::string content; // String to hold the contents of the file.

    // Check if the file stream was successfully opened.
    if (!fileStream.is_open())
    {
        // If the file cannot be opened (e.g., does not exist), print an error message.
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return ""; // Return an empty string as an error indication.
    }

    // Use a string stream to read the entire contents of the file into the 'content' string.
    std::stringstream sstr;
    sstr << fileStream.rdbuf();
    content = sstr.str();
    fileStream.close(); // Close the file stream.

    return content; // Return the contents of the file as a string.
}

// Function to compile a shader from source code.
// type: The type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
// source: The shader source code as a string.
unsigned int compileShader(unsigned int type, const std::string &source)
{
    // Create a shader object.
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();     // Convert the source string to a C-style string.
    glShaderSource(id, 1, &src, nullptr); // Attach the shader source code to the shader object.
    glCompileShader(id);                  // Compile the shader.

    // Check for compilation errors.
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        // If an error occurred during compilation, query the error message length.
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        // Allocate memory on the stack for the error message.
        char *message = (char *)alloca(length * sizeof(char));
        // Retrieve the error message.
        glGetShaderInfoLog(id, length, &length, message);
        // Print the error message.
        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!\n"
                  << message << std::endl;
        glDeleteShader(id); // Delete the shader object to free resources.
        return 0;           // Return 0 as an error indication.
    }

    return id; // Return the shader object ID.
}

// Function to create a shader program by linking a vertex and a fragment shader.
// vertexShader: The vertex shader source code as a string.
// fragmentShader: The fragment shader source code as a string.
unsigned int createShaderProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
    // Create a shader program object.
    unsigned int program = glCreateProgram();
    // Compile the vertex and fragment shaders from their source code.
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // Attach the compiled shaders to the shader program.
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    // Link the shader program to create an executable for the GPU.
    glLinkProgram(program);
    // Perform validation on the shader program.
    glValidateProgram(program);

    // Delete the shader objects now that they are linked into the program; they are no longer needed.
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program; // Return the shader program object ID.
}