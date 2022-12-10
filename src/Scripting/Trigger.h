#ifndef TRIGGER_H
#define TRIGGER_H

#include "Components.h"

#include "Core/Event.h"


class TriggerEvent : public Event
{
public:
    TriggerEvent(const Entity& triggered, const Entity& source) : 
            Event(), m_triggered(triggered), m_source(source) {}

    inline Entity GetTriggered() const { return m_triggered; }
    inline Entity GetSource() const { return m_source; }

    DEFINE_EVENT_CATEGORY(Game)

private:
    Entity m_triggered;
    Entity m_source;
};

class TriggerEnterEvent final : public TriggerEvent
{
public:
    TriggerEnterEvent(const Entity& triggered, const Entity& source) : 
            TriggerEvent(triggered, source) {}

    inline uint32_t GetType() const { return TriggerEnterEvent::TypeId; }
    inline const char* GetName() const { return "TriggerEnterEvent"; }
    static const uint32_t TypeId = 0x401;
};

class TriggerStayEvent final : public TriggerEvent
{
public:
    TriggerStayEvent(const Entity& triggered, const Entity& source) : 
            TriggerEvent(triggered, source) {}

    inline uint32_t GetType() const { return TriggerStayEvent::TypeId; }
    inline const char* GetName() const { return "TriggerStayEvent"; }
    static const uint32_t TypeId = 0x402;
};

class TriggerExitEvent final : public TriggerEvent
{
public:
    TriggerExitEvent(const Entity& triggered, const Entity& source) : 
            TriggerEvent(triggered, source) {}

    inline uint32_t GetType() const { return TriggerExitEvent::TypeId; }
    inline const char* GetName() const { return "TriggerExitEvent"; }
    static const uint32_t TypeId = 0x403;
};


namespace Components {


class Trigger : public Scripted
{
public:
    Trigger(const Entity& entity, const Entity& target, const float& radius = 0.5f) :
            Scripted("Trigger", entity),
            m_target(target), 
            m_radius(radius) {}

    ~Trigger() = default;

    void OnUpdate() override;

    inline Entity GetTarget() { return m_target; }
    inline void SetTarget(const Entity& target) { m_target = target; }

private:
    Entity m_target;
    float m_radius;
};


} // Namespace Components


#endif // TRIGGER_H