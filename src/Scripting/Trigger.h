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
    static const uint32_t TypeId = 0x401;
};

class TriggerStayEvent final : public TriggerEvent
{
public:
    TriggerStayEvent(const Entity& triggered, const Entity& source) : 
            TriggerEvent(triggered, source) {}

    inline uint32_t GetType() const { return TriggerStayEvent::TypeId; }
    static const uint32_t TypeId = 0x402;
};

class TriggerExitEvent final : public TriggerEvent
{
public:
    TriggerExitEvent(const Entity& triggered, const Entity& source) : 
            TriggerEvent(triggered, source) {}

    inline uint32_t GetType() const { return TriggerExitEvent::TypeId; }
    static const uint32_t TypeId = 0x403;
};


namespace Components {


class Trigger : public Scripted
{
public:
    Trigger(const Entity& target, const float& radius = 0.5) :
            m_target(target), m_radius(radius) {}
    ~Trigger() = default;

    void OnUpdate();

private:
    Entity m_target;
    float m_radius;
};


} // Namespace Components


#endif // TRIGGER_H