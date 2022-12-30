#include "Components.h"

#include "Engine.h"

#include "Game/GameEvents.h"


namespace Components {


// == Scripted == 

Scripted::Scripted(const std::string& name, const Entity& entity) : 
        m_name(name), 
        m_entity(entity)
{
    Scripting::Engine::Get().Register(this);
}

Scripted::Scripted(const Scripted& other) :
        m_name(other.m_name), 
        m_entity(other.m_entity)
{
    Scripting::Engine::Get().Register(this);
}

Scripted::~Scripted()
{
    Scripting::Engine::Get().Deregister(this);
}


// == Scriptable ==

Scriptable::Scriptable(const std::string& name,
                       const Entity& entity,
                       const OnCreateFn& onCreate,
                       const OnUpdateFn& onUpdate,
                       const OnEventFn& onEvent,
                       const OnDestroyFn& onDestroy) : 
        Scripted(name, entity),
        m_onCreateFn(onCreate), 
        m_onUpdateFn(onUpdate),
        m_onEventFn(onEvent),
        m_onDestroyFn(onDestroy)
{
    OnCreate();
}

Scriptable::Scriptable(const Scriptable& other) :
        Scripted(other),
        m_dataBlock(other.m_dataBlock),
        m_onCreateFn(other.m_onCreateFn), 
        m_onUpdateFn(other.m_onUpdateFn),
        m_onEventFn(other.m_onEventFn),
        m_onDestroyFn(other.m_onDestroyFn)
{
    OnCreate();
}

Scriptable::~Scriptable()
{
    OnDestroy();
}

void Scriptable::OnCreate() 
{
    if (m_onCreateFn)
        m_onCreateFn(GetEntity(), m_dataBlock);
}

void Scriptable::OnUpdate()
{
    if (m_onUpdateFn)
        m_onUpdateFn(GetEntity(), m_dataBlock);
}

void Scriptable::OnEvent(GameEvent* event)
{
    if (m_onEventFn)
        m_onEventFn(event, GetEntity(), m_dataBlock);
}

void Scriptable::OnDestroy()
{
    if (m_onDestroyFn)
        m_onDestroyFn(GetEntity(), m_dataBlock);
}


} // Namespace Components
