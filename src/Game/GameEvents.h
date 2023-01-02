#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include "Components.h"
#include "Attack.h"

#include "Core/Event.h"


// == Game Event ==

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


// == Trigger Events ==

class TriggerEvent : public GameEvent
{
public:
    TriggerEvent(const Entity& triggered, const Entity& source) : 
            GameEvent(triggered), m_source(source) {}

    inline Entity GetSource() const { return m_source; }

    inline const char* GetName() const { return "TriggerEvent"; }
    inline uint32_t GetType() const { return 0; }

    DEFINE_EVENT_CATEGORY(Game)

private:
    Entity m_source;
};

class TriggerEnterEvent final : public TriggerEvent
{
public:
    TriggerEnterEvent(const Entity& triggered, const Entity& source) : 
            TriggerEvent(triggered, source) {}

    inline const char* GetName() const { return "TriggerEnterEvent"; }
    inline uint32_t GetType() const { return TriggerEnterEvent::TypeId; }
    static const uint32_t TypeId = 0x401;
};

class TriggerStayEvent final : public TriggerEvent
{
public:
    TriggerStayEvent(const Entity& triggered, const Entity& source) : 
            TriggerEvent(triggered, source) {}

    inline const char* GetName() const { return "TriggerStayEvent"; }
    inline uint32_t GetType() const { return TriggerStayEvent::TypeId; }
    static const uint32_t TypeId = 0x402;
};

class TriggerExitEvent final : public TriggerEvent
{
public:
    TriggerExitEvent(const Entity& triggered, const Entity& source) : 
            TriggerEvent(triggered, source) {}

    inline const char* GetName() const { return "TriggerExitEvent"; }
    inline uint32_t GetType() const { return TriggerExitEvent::TypeId; }
    static const uint32_t TypeId = 0x403;
};

// == Attack Event ==

class AttackEvent : public GameEvent
{
public:
    AttackEvent(const Entity& target, const Attack& attack) : 
            GameEvent(target), m_attack(attack) {}

    inline const char* GetName() const { return "AttackEvent"; }
    inline uint32_t GetType() const { return AttackEvent::TypeId; }
    static const uint32_t TypeId = 0x501;

    inline const Attack& GetAttack() const { return m_attack; }

private:
    const Attack m_attack;
};


// == Heal Event ==

class HealEvent : public GameEvent
{
public:
    HealEvent(const Entity& target, const float& healing) : 
            GameEvent(target), m_healing(healing) {}

    inline const char* GetName() const { return "HealEvent"; }
    inline uint32_t GetType() const { return HealEvent::TypeId; }
    static const uint32_t TypeId = 0x502;

    inline float GetHealing() const { return m_healing; }

private:
    const float m_healing;
};


// == Pickup Weapon Event ==


class PickupWeaponEvent : public GameEvent
{
public:
    PickupWeaponEvent(const Entity& target, const Components::WeaponData& weapon) : 
            GameEvent(target), m_weapon(weapon) {}

    inline const char* GetName() const { return "PickupWeaponEvent"; }
    inline uint32_t GetType() const { return PickupWeaponEvent::TypeId; }
    static const uint32_t TypeId = 0x503;

    inline const Components::WeaponData& GetWeapon() const { return m_weapon; }

private:
    Components::WeaponData m_weapon;
};

#endif // GAMEEVENTS_H
