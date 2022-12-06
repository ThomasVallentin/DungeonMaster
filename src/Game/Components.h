#ifndef GAMECOMPONENTS_H
#define GAMECOMPONENTS_H

#include "Scene/Components/Basics.h"

#include "Navigation/Engine.h"


namespace Components {

struct CharacterControllerData
{
    Animation<glm::mat4> moveAnimation;
    Animation<glm::mat4> attackAnimation;
    float speed = 2.0f;
    Navigation::CellFilters navFilter = Navigation::CellFilters::Default;
};

Scriptable CreateCharacterController(const Entity& entity);

struct MonsterLogicData
{
    float speed = 2.0f;
    float angleOfView = 120.0f;
    float viewDistance = 4.0f;
    Navigation::CellFilters navFilter = Navigation::CellFilters::Default;
    Entity target;
};

Scriptable CreateMonsterLogic(const Entity& entity);

} // Namespace Components


#endif // GAMECOMPONENTS_H