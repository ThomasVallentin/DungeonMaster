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

    inline glm::vec3 GetSideVector() const  { return transform[0]; }
    inline glm::vec3 GetUpVector() const    { return transform[1]; }
    inline glm::vec3 GetFrontVector() const { return transform[2]; }
    inline glm::vec3 GetPosition() const    { return transform[3]; }   

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
    RenderMesh(const ResourceHandle<Material>& material, const bool& doubleSided) : 
            material(material), 
            doubleSided(doubleSided) {}

    ResourceHandle<Material> material;
    bool doubleSided = false;
};


struct RenderImage
{
    RenderImage() = default;
    RenderImage(const ResourceHandle<Texture>& image) : image(image) {}

    ResourceHandle<Texture> image;
};

} // Namespace Components::


#endif  // BASICCOMPONENTS_H
