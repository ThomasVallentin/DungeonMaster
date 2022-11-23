#ifndef BASICCOMPONENTS_H
#define BASICCOMPONENTS_H


#include "Scene/Entity.h"
#include "Scene/ScriptEngine.h"

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

    glm::mat4 transform{1.0f};
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


typedef std::function<void(const Entity&, std::any&)>         OnCreateFn;
typedef std::function<void(const Entity&, std::any&)>         OnUpdateFn;
typedef std::function<void(Event*, const Entity&, std::any&)> OnEventFn;


class Script 
{
public:
    Script() = default;
    Script(const std::string& name,
           const Entity& entity,
           const OnCreateFn& onCreate,
           const OnUpdateFn& onUpdate,
           const OnEventFn& onEvent) : 
            m_name(name), 
            m_entity(entity), 
            m_onCreateFn(onCreate), 
            m_onUpdateFn(onUpdate),
            m_onEventFn(onEvent) 
    {
        ScriptEngine::Get().Register(this);
        OnCreate();
    }

    Script(const Script& other) :
           m_name(other.m_name), 
           m_dataBlock(other.m_dataBlock),
           m_entity(other.m_entity), 
           m_onCreateFn(other.m_onCreateFn), 
           m_onUpdateFn(other.m_onUpdateFn),
           m_onEventFn(other.m_onEventFn)
    {
        ScriptEngine::Get().Register(this);
    }

    ~Script()
    {
        ScriptEngine::Get().Deregister(this);
    }

    inline const std::string& GetName() const 
    {
        return m_name;
    }

    inline void OnCreate() 
    {
        if (m_onCreateFn)
            m_onCreateFn(m_entity, m_dataBlock);
    }

    inline void OnUpdate()
    {
        if (m_onUpdateFn)
            m_onUpdateFn(m_entity, m_dataBlock);
    }

    inline void OnEvent(Event* event)
    {
        if (m_onEventFn)
            m_onEventFn(event, m_entity, m_dataBlock);
    }

private:
    std::string m_name; 
    std::any m_dataBlock;
    Entity m_entity;

    OnCreateFn m_onCreateFn;
    OnUpdateFn m_onUpdateFn;
    OnEventFn m_onEventFn;
};


struct CameraComponent
{
    CameraComponent() = default;
    CameraComponent(const ProjectionSpecs& specs) : specs(specs) {}

    ProjectionSpecs specs;
};


} // Namespace Components::


#endif  // BASICCOMPONENTS_H
