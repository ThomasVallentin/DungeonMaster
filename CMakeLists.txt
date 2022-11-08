cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)

if(POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif()
if(POLICY CMP0003)
    cmake_policy(SET CMP0003 NEW)
endif()

project(DungeonMaster VERSION 0.0.1 LANGUAGES CXX C)

add_subdirectory(third-party)

set(DungeonMaster_SOURCES src/main.cpp
                          src/Core/Application.cpp
                          src/Core/Window.cpp
                          src/Core/Image.cpp
                          src/Utils/FileUtils.cpp
                          src/Utils/TypeUtils.cpp
                          src/Renderer/Mesh.cpp
                          src/Renderer/Shader.cpp
                          src/Renderer/Texture.cpp
                          src/Renderer/VertexBuffer.cpp
                          src/Renderer/VertexArray.cpp
)

add_executable(DungeonMaster ${DungeonMaster_SOURCES})
target_include_directories(DungeonMaster PUBLIC 
                           src/)

target_link_libraries(DungeonMaster PUBLIC 
                      glad
                      glfw
                      glm
                      stb
                      assimp)
