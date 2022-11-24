#include "Basics.h"


namespace Components {


// == Transform ==

glm::mat4 Transform::ComputeWorldMatrix(const Entity& entity)
{
    glm::mat4 result = glm::mat4(1.0f);

    Entity parent = entity;
    while (parent)
    {
        if (auto* transform = parent.FindComponent<Components::Transform>())
        {
            result = transform->transform * result;
        }
        parent = parent.GetParent();
    }

    return result;
}


// == Script ==

Script::Script(const std::string& name,
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

Script::Script(const Script& other) :
        m_name(other.m_name), 
        m_dataBlock(other.m_dataBlock),
        m_entity(other.m_entity), 
        m_onCreateFn(other.m_onCreateFn), 
        m_onUpdateFn(other.m_onUpdateFn),
        m_onEventFn(other.m_onEventFn)
{
    ScriptEngine::Get().Register(this);
}

Script::~Script()
{
    ScriptEngine::Get().Deregister(this);
}

const std::string& Script::GetName() const 
{
    return m_name;
}

void Script::OnCreate() 
{
    if (m_onCreateFn)
        m_onCreateFn(m_entity, m_dataBlock);
}

void Script::OnUpdate()
{
    if (m_onUpdateFn)
        m_onUpdateFn(m_entity, m_dataBlock);
}

void Script::OnEvent(Event* event)
{
    if (m_onEventFn)
        m_onEventFn(event, m_entity, m_dataBlock);
}


}  // Namespace Components