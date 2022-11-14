#ifndef SCRIPTEDCOMPONENT_H
#define SCRIPTEDCOMPONENT_H


#include "Entity.h"

#include "Core/Event.h"
#include "Core/Logging.h"

#include "ResourceManagement/ResourceManager.h"

#include <functional>


struct TransformComponent 
{
    glm::vec3 translate = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotate = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
};

struct MeshComponent
{
    ResourceHandle<Mesh> mesh;
};



struct RenderMeshComponent
{
    // ResourceHandle<Material> material;
    // bool castShadows;
    // bool receiveShadows;
};


class ScriptedComponent 
{
public:
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


#endif // SCRIPTEDCOMPONENT_H
