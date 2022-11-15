cmake_minimum_required(VERSION 3.12)

# == RapidJSON ==
add_library(RapidJSON INTERFACE)
target_include_directories(RapidJSON INTERFACE RapidJSON/include)

# == GLFW ==
add_subdirectory(GLFW)

# == glad ==
add_library(glad glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# == glm ==
add_subdirectory(glm)

# == stb == 
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb/)

# == assimp ==

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)  # Disable the warning since these are 
endif()
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)

add_subdirectory(assimp)
