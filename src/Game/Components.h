#ifndef GAMECOMPONENTS_H
#define GAMECOMPONENTS_H

#include "Navigation/Engine.h"

#include "Scripting/Components.h"

#include "Scene/Components/Basics.h"


namespace Components {

struct CharacterData
{
    CharacterData() = default;
    CharacterData(const float& health) : health(health), maxHealth(health) {}

    inline void InflictDamage(const float& damage)
    {
        health = std::max(0.0f, health - damage);
    }

    inline void Heal(const float& healing)
    {
        health = std::min(maxHealth, health + healing);
    }

    inline bool IsAlive() const { return health > 0.0f; }
    
    float health = 10.0f;
    float maxHealth = 10.0f;
};

struct CharacterControllerData
{
    Animation<glm::vec4> haloEffectAnimation;
    Animation<glm::mat4> moveAnimation;
    Animation<glm::mat4> attackAnimation;
    float speed = 4.0f;
    bool hasAttacked = false;
    Navigation::CellFilters navFilter = Navigation::CellFilters::Default;
};

Scriptable CreateCharacterController(const Entity& entity);

struct MonsterData
{
    MonsterData() = default;
    MonsterData(const Entity& target, 
                const float& strength,
                const float& attackSpeed) :
            target(target),
            attackSpeed(attackSpeed),
            strength(strength) {}

    Entity target;
    float attackSpeed = 2.0f;
    float strength = 1.0;
    float angleOfView = 120.0f;
    float viewDistance = 4.0f;

// Scripts cannot access to private variables in the current state of the engine, letting everything public for now
// private: 
    double attackDelay = 0.0;
};

Scriptable CreateMonsterLogic(const Entity& entity);


struct RewardAnimatorData
{
    Animation<glm::vec3> translateAnimation;
    Animation<float> rotateAnimation;
};

Scriptable CreateRewardAnimator(const Entity& entity);


struct HealData
{
    float healing = 2.0f;
};

Scriptable CreateHealLogic(const Entity& entity);


struct WeaponData
{
    WeaponData() = default;
    WeaponData(const float& damage, 
               const float& speed, 
               const std::string& modelIdentifier) : 
            damage(damage),
            speed(speed),
            modelIdentifier(modelIdentifier) {}

    inline float GetDPS() const { return damage * speed; }

    float damage = 2.0f;
    float speed = 1.0f;
    float range = 1.2f;  // Could be usefull later

    std::string modelIdentifier;
};


Scriptable CreateWeaponLogic(const Entity& entity);


struct DoorData
{
    bool opened = false;
    Animation<glm::mat4> openAnimation;
};

Scriptable CreateDoorLogic(const Entity& entity);


// General game logics
Scriptable CreateTitleScreenLogic(const Entity& entity);
Scriptable CreateGameOverLogic(const Entity& entity);
Scriptable CreateExitLogic(const Entity& entity);
Scriptable CreateEndScreenLogic(const Entity& entity);


} // Namespace Components


#endif // GAMECOMPONENTS_H