#ifndef BASICCOMPONENTS_H
#define BASICCOMPONENTS_H


#include "Scene/Entity.h"
#include "Scripting/Engine.h"

#include "Core/Event.h"
#include "Core/Logging.h"

#include "Resources/Manager.h"

#include "Renderer/Camera.h"

#include <functional>
#include <any>


namespace Components {


struct Transform 
{
    Transform() = default;
    Transform(const glm::mat4& transform) : transform(transform) {}

    static glm::mat4 ComputeWorldMatrix(const Entity& entity);

    glm::mat4 transform{1.0f};
};


struct Camera
{
    Camera() = default;
    Camera(const ::Camera& camera) : camera(camera) {}

    ::Camera camera;
};

struct Mesh
{
    Mesh() = default;
    Mesh(const ResourceHandle<::Mesh>& mesh) : mesh(mesh) {}

    ResourceHandle<::Mesh> mesh;
};

struct RenderMesh
{
    RenderMesh() = default;
    RenderMesh(const ResourceHandle<Material>& material) : material(material) {}

    ResourceHandle<Material> material;
    // bool castShadows;
    // bool receiveShadows;
};

} // Namespace Components::


#endif  // BASICCOMPONENTS_H
