cmake_minimum_required(VERSION 3.12)

# == GLFW ==
add_subdirectory(GLFW)

# == glad ==
add_library(glad glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# == glm ==
add_subdirectory(glm)
