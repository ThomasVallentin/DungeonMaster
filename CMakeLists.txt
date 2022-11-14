cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)

if(POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif()
if(POLICY CMP0003)
    cmake_policy(SET CMP0003 NEW)
endif()

project(DungeonMaster VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_OUTPUT ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(third-party)

# Define the executable name. DungeonMaster for Release DungeonMaster-Config for every other config
set(DungeonMaster_EXE DungeonMaster)
if(NOT ${CMAKE_BUILD_TYPE} STREQUAL Release)
    set(DungeonMaster_EXE ${DungeonMaster_EXE}-${CMAKE_BUILD_TYPE})
endif()

set(DungeonMaster_SOURCES src/main.cpp

                          src/Core/Application.cpp
                          src/Core/Image.cpp
                          src/Core/Resolver.cpp
                          src/Core/Window.cpp


                          src/Renderer/Camera.cpp
                          src/Renderer/FrameBuffer.cpp
                          src/Renderer/Material.cpp
                          src/Renderer/Mesh.cpp
                          src/Renderer/Shader.cpp
                          src/Renderer/Texture.cpp
                          src/Renderer/UniformBuffer.cpp
                          src/Renderer/VertexArray.cpp
                          src/Renderer/VertexBuffer.cpp
                          
                          src/Game/Entity.cpp
                          src/Game/Scene.cpp
                          src/Game/EntityIndex.cpp
                          
                          src/ResourceManagement/Model.cpp
                          src/ResourceManagement/ResourceManager.cpp
                          
                          src/Utils/FileUtils.cpp
                          src/Utils/TypeUtils.cpp
                          
)

add_executable(${DungeonMaster_EXE} ${DungeonMaster_SOURCES})
target_include_directories(${DungeonMaster_EXE} PUBLIC 
                           src/)

target_link_libraries(${DungeonMaster_EXE} PUBLIC 
                      glad
                      glfw
                      glm
                      stb
                      assimp)
