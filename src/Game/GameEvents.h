#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include "Core/Event.h"


// == GameEvent ==

class GameEvent : public Event
{
public:
    GameEvent(const Entity& entity) : m_entity(entity) {}

    inline uint32_t GetType() const { return 0; }
    inline const char* GetName() const { return "GameEvent"; }

    DEFINE_EVENT_CATEGORY(Game)

    inline Entity GetEntity() const { return m_entity; }

private:
    Entity m_entity;
};


// == TriggerEvents ==

class TriggerEvent : public GameEvent
{
public:
    TriggerEvent(const Entity& triggered, const Entity& source) : 
            GameEvent(triggered), m_source(source) {}

    inline Entity GetSource() const { return m_source; }

    inline uint32_t GetType() const { return 0; }
    inline const char* GetName() const { return "TriggerEvent"; }

    DEFINE_EVENT_CATEGORY(Game)

private:
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

// == AttackEvent ==

// class AttackEvent : public GameEvent
// {
// public:
//     AttackEvent(const WeaponData& data, const Entity& target) : 
//             Event() {}

//     inline uint32_t GetType() const { return 0x501; }
//     inline const char* GetName() const { return "DamageEvent"; }

// private:
//     float m_damage;

// };

#endif // GAMEEVENTS_H