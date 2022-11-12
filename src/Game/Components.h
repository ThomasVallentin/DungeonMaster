#ifndef SCRIPTEDCOMPONENT_H
#define SCRIPTEDCOMPONENT_H


#include "Entity.h"

#include "Core/Event.h"
#include "Core/Logging.h"


#include <functional>


class ScriptedComponent 
{
public:
    ScriptedComponent(const Entity& entity) : m_entity(entity) {}

    virtual void OnUpdate() = 0;
    virtual void OnEvent(Event* event) = 0;

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
