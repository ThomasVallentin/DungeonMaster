#ifndef BASICCOMPONENTS_H
#define BASICCOMPONENTS_H


#include "Scene/Entity.h"

#include "Core/Event.h"
#include "Core/Logging.h"

#include "Resources/Manager.h"

#include <functional>


struct TransformComponent 
{
    TransformComponent() = default;
    TransformComponent(const glm::mat4& transform) : transform(transform) {}

    glm::mat4 transform{1.0f};
};

struct MeshComponent
{
    MeshComponent() = default;
    MeshComponent(const ResourceHandle<Mesh>& mesh) : mesh(mesh) {}

    ResourceHandle<Mesh> mesh;
};



struct RenderMeshComponent
{
    RenderMeshComponent() = default;
    RenderMeshComponent(const ResourceHandle<Material>& material) : material(material) {}

    ResourceHandle<Material> material;
    // bool castShadows;
    // bool receiveShadows;
};


class ScriptedComponent 
{
public:
    ScriptedComponent() = default;
    ScriptedComponent(const Entity& entity) : m_entity(entity) {}

    virtual void OnCreate() {};
    virtual void OnUpdate() {};
    virtual void OnEvent(Event* event) {};
    virtual void OnRemove() {};

protected:
    Entity m_entity;
};


class DebugComponent : public ScriptedComponent 
{
public:
    DebugComponent(const Entity& entity) : ScriptedComponent(entity) {}

    void OnUpdate() override {
        LOG_INFO("Updating entity %s", m_entity.GetName().c_str());
    }

    void OnEvent(Event* event) override {
        LOG_INFO("Entity %s received event %s", m_entity.GetName().c_str(), event->GetName());
    }
};


#endif // BASICCOMPONENTS_H
