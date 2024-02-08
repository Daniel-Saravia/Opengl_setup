cmake_minimum_required(VERSION 3.0.0)
project(my_opengl_project VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

# Find the required packages
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED) # Add this line to find the GLM package

# Add your executable
add_executable(my_opengl_project
    src/main.cpp 
    src/glad.c
    src/glad.h
)

# Since GLM is header only, it does not need to be linked like a traditional library.
# However, specifying it ensures your CMakeLists is clear on its dependencies.

# Include directories for your target
target_include_directories(my_opengl_project PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GLM_INCLUDE_DIRS} # Ensure GLM's include path is added
)

# Link libraries
target_link_libraries(my_opengl_project 
    glfw
    OpenGL::GL
    # No need to explicitly link GLM since it's header-only
)

# If you're using a specific C++ standard, specify it here
set_property(TARGET my_opengl_project PROPERTY CXX_STANDARD 17)
