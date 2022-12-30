#ifndef GAMECOMPONENTS_H
#define GAMECOMPONENTS_H

#include "Scene/Components/Basics.h"

#include "Navigation/Engine.h"


namespace Components {

struct CharacterData
{
    CharacterData() = default;
    CharacterData(const float& health) : health(health) {}

    inline void InflictDamage(const float& damage)
    {
        health = std::max(0.0f, health - damage);
    }

    inline bool IsAlive() const { return health > 0.0f; }
    
    float health = 10.0f;
};

struct CharacterControllerData
{
    Animation<glm::mat4> moveAnimation;
    Animation<glm::mat4> attackAnimation;
    float speed = 4.0f;
    bool hasAttacked = false;
    Navigation::CellFilters navFilter = Navigation::CellFilters::Default;
};

Scriptable CreateCharacterController(const Entity& entity);

struct MonsterData
{
    float speed = 2.0f;
    float strength = 1.0;
    float angleOfView = 120.0f;
    float viewDistance = 4.0f;
    Navigation::CellFilters navFilter = Navigation::CellFilters::Flying;
    Entity target;
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
    WeaponData(const float& damage) : damage(damage) {}

    float damage = 2.0f;
    float speed = 1.0f;
    float range = 1.2f;
};


Scriptable CreateWeaponLogic(const Entity& entity);


} // Namespace Components


#endif // GAMECOMPONENTS_H