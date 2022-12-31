#include "Components.h"

#include "GameEvents.h"
#include "GameManager.h"

#include "Navigation/Components.h"

#include "Scripting/Trigger.h"

#include "Core/Event.h"
#include "Core/Inputs.h"
#include "Core/Logging.h"
#include "Core/Time.h"
#include "Core/Animation.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>


namespace Components {


// == Character Controller ==


Scriptable CreateCharacterController(const Entity& entity)
{
    return Scriptable(
        "CharacterController",
        entity,

// CharacterController::OnCreate
[](Entity entity, std::any& dataBlock)
{
    dataBlock = std::make_any<CharacterControllerData>();
},

// CharacterController::OnUpdate
[](Entity entity, std::any& dataBlock)
{
    auto* transform = entity.FindComponent<Transform>();
    if (!transform)
    {
        return;
    }

    CharacterControllerData& data = std::any_cast<CharacterControllerData&>(dataBlock);

    bool shouldSampleInput = true;

    // The character is currently moving, evaluate the move animation
    if (!data.moveAnimation.ended)
    {
        transform->transform = data.moveAnimation.Evaluate(Time::GetDeltaTime());
        shouldSampleInput = false;
    }

    // The character is currently attacking, evaluate the move animation
    if (!data.attackAnimation.ended)
    {
        Entity weapon = entity.FindChild("Weapon");
        auto* childTransform = weapon.FindComponent<Transform>();
        if (childTransform)
        {
            childTransform->transform = data.attackAnimation.Evaluate(Time::GetDeltaTime());

            // Hack: The attack is really happening here to make it match with the animation
            if (!data.hasAttacked && data.attackAnimation.currentTime > 0.4f)
            {
                data.hasAttacked = true;
                auto& weaponData = weapon.GetComponent<WeaponData>();
                glm::mat4 worldMatrix = Transform::ComputeWorldMatrix(entity);
                PerformAttack(Attack{weaponData.damage,
                                     worldMatrix[3],
                                     -worldMatrix[2],
                                     weaponData.range,
                                     60.0f}, 
                              GameManager::Get().GetMonsters());
            }
        }
        shouldSampleInput = false;
    }

    if (!shouldSampleInput)
    {
        return;
    }
    
    Navigation::Engine& navEngine = Navigation::Engine::Get();

    // Sample keyboard inputs
    if (Inputs::IsKeyPressed(KeyCode::Up))
    {
        glm::mat4 nextTransform = glm::translate(transform->transform, glm::vec3(0, 0, -1.0f));
        glm::vec2 nextCell = glm::vec2(nextTransform[3].x, nextTransform[3].z);
        if (navEngine.CellIsEmpty(nextCell, data.navFilter))
        {
            data.moveAnimation = {{{0.0f, transform->transform},
                                   {1.0f, nextTransform}},
                                  InterpolationType::Smooth,
                                  data.speed};
            data.moveAnimation.Start();
        }
    }
    else if (Inputs::IsKeyPressed(KeyCode::Down))
    {
        glm::mat4 nextTransform = glm::translate(transform->transform, glm::vec3(0, 0, 1.0f));
        glm::vec2 nextCell = glm::vec2(nextTransform[3].x, nextTransform[3].z);
        if (navEngine.CellIsEmpty(nextCell, data.navFilter))
        {
            data.moveAnimation = {{{0.0f, transform->transform},
                                   {1.0f, nextTransform}},
                                  InterpolationType::Smooth,
                                  data.speed};
            data.moveAnimation.Start();
        }
    }
    else if (Inputs::IsKeyPressed(KeyCode::Left))
    {
        data.moveAnimation = {{{0.0f, transform->transform},
                               {1.0f, glm::rotate(transform->transform, (float)M_PI_2, glm::vec3(0, 1, 0))}},
                              InterpolationType::Smooth,
                              data.speed};
        data.moveAnimation.Start();
    }
    else if (Inputs::IsKeyPressed(KeyCode::Right))
    {
        data.moveAnimation = {{{0.0f, transform->transform},
                               {1.0f, glm::rotate(transform->transform, -(float)M_PI_2, glm::vec3(0, 1, 0))}},
                              InterpolationType::Smooth,
                              data.speed};
        data.moveAnimation.Start();
    }

    // Sampling mouse inputs
    else if (Inputs::IsMouseButtonPressed(MouseButton::Left))
    {
        auto* childTransform = entity.FindChild("Weapon").FindComponent<Transform>();
        if (childTransform)
        {
            data.attackAnimation = {{
                    {0.0f, childTransform->transform},
                    {0.2f, glm::rotate(childTransform->transform, -(float)M_PI / 8.0f, glm::vec3(0, 0, 1))},
                    {0.4f, glm::rotate(childTransform->transform, (float)M_PI / 4.0f, glm::vec3(0, 0, 1))},
                    {1.0f, childTransform->transform}
                },
                InterpolationType::Smooth, 
                1.5f
            };
            data.attackAnimation.Start();
            data.hasAttacked = false;
        }
    }
},

// CharacterController::OnEvent
[](GameEvent* event, Entity entity, std::any& dataBlock)
{
    switch (event->GetType())
    {
        case AttackEvent::TypeId:
        {
            auto* character = entity.FindComponent<CharacterData>();
            if (!character)
            {
                return;
            }
            auto* attackEvent = dynamic_cast<AttackEvent*>(event);
            character->InflictDamage(attackEvent->GetAttack().damage);

            if (!character->IsAlive())
            {
                entity.Remove();
            }
        }
    }
},

// CharacterController::OnDestroy
[](Entity entity, std::any& dataBlock)
{
    GameManager::Get().RemoveMonster(entity);
}

);

} 

// == Moster Logic ==

Scriptable CreateMonsterLogic(const Entity& entity)
{
    return Scriptable(
        "MonsterLogic",
        entity,

// MonsterLogic::OnCreate
[](Entity entity, std::any& dataBlock)
{
    GameManager::Get().AddMonster(entity);
},

// MonsterLogic::OnUpdate
[](Entity entity, std::any& dataBlock)
{
    NavAgent* navAgent = entity.FindComponent<NavAgent>();
    auto agent = navAgent->GetAgent();
    if (!navAgent)
    {
        return;
    }

    MonsterData& data = entity.GetComponent<MonsterData>();
    Transform* transform = entity.FindComponent<Transform>();
    if (!transform || !data.target)
    {
        return;
    }

    Transform* targetTransform = data.target.FindComponent<Transform>();
    if (!targetTransform)
    {
        return;
    }

    glm::vec2 pos = {round(transform->transform[3].x), round(transform->transform[3].z)};
    glm::vec2 targetPos = {round(targetTransform->transform[3].x), round(targetTransform->transform[3].z)};
    glm::vec2 toTarget = targetPos - pos;

    // Target is too far away
    if (glm::length2(toTarget) > (data.viewDistance * data.viewDistance))
    {
        return;
    }

    glm::vec2 viewDir = glm::normalize(glm::vec2(transform->transform[2].x, transform->transform[2].z));
    glm::vec2 toTargetDir = glm::normalize(toTarget);

    // Target is not in the angle of view
    if (std::max(glm::dot(viewDir, toTargetDir), 0.0f) < (std::cos(glm::radians(data.angleOfView * 0.5f))))
    {
        return;
    }

    Navigation::Engine& navEngine = Navigation::Engine::Get();
    glm::vec2 searchPos = targetPos;
    if (std::abs(toTargetDir.x) > std::abs(toTargetDir.y))
    {
        // Normalizing the direction by its x
        toTargetDir.y /= toTargetDir.x;
        toTargetDir.x = 1.0f;

        // Make sure that pos has a smaller x than searchPos
        if (pos.x > searchPos.x)
        {
            std::swap(searchPos, pos);
            toTargetDir *= -1.0f;
        }

        int ySign = toTargetDir.y > 0 ? 1 : -1;
        float e = 0.0f;
        while (pos.x <= searchPos.x)
        {
            if (std::abs(e) >= 0.5f)
            {
                if (!navEngine.CellIsEmpty(pos, data.navFilter))
                    return;
                pos.y += ySign;
                e = ySign - e;
            }

            if (!navEngine.CellIsEmpty(pos, data.navFilter))
                return;
            e -= toTargetDir.y;
            pos.x++;
        }
    }
    else
    {
        // Normalizing the direction by its y
        toTargetDir.x /= toTargetDir.y;
        toTargetDir.y = 1.0f;

        // Make sure that pos has a smaller x than searchPos
        if (pos.y > searchPos.y)
        {
            std::swap(searchPos, pos);
            toTargetDir *= -1.0;
        }

        int xSign = toTargetDir.x > 0 ? 1 : -1;
        float e = 0;
        while (pos.y <= searchPos.y)
        {
            if (std::abs(e) >= 0.5)
            {
                if (!navEngine.CellIsEmpty(pos, data.navFilter))
                    return;
                pos.x += xSign;
                e = xSign - e;
            }

            if (!navEngine.CellIsEmpty(pos, data.navFilter))
                return;
            pos.y++;
            e -= toTargetDir.x;
        }
    }

    navAgent->GetAgent()->SetDestination(glm::vec3(targetPos.x, 0.0f, targetPos.y));
},

// MonsterLogic::OnEvent
[](Event* event, Entity entity, std::any& dataBlock) 
{
    switch (event->GetType())
    {
        case AttackEvent::TypeId:
        {
            auto& characterData = entity.GetComponent<CharacterData>();
            const Attack& attack = dynamic_cast<AttackEvent*>(event)->GetAttack();

            characterData.InflictDamage(attack.damage);
            if (!characterData.IsAlive())
            {
                entity.Remove();
                return;
            }
            // TODO: Insert on hit effect here 
            // else
            // {
            // }
        }
    }
},

// MonsterLogic::OnDestroy
[](Entity entity, std::any& dataBlock)
{
    GameManager::Get().RemoveMonster(entity);
}

);

} 


Scriptable CreateRewardAnimator(const Entity& entity)
{
    return Scriptable(
        "RewardAnimator",
        entity,

// RewardAnimator::OnCreate
[](Entity entity, std::any& dataBlock)
{
    Animation<glm::vec3> translate = {{{0.0f, glm::vec3(0.0f, -0.10f, 0.0f)},
                                       {0.5f, glm::vec3(0.0f, -0.15f, 0.0f)},
                                       {1.0f, glm::vec3(0.0f, -0.10f, 0.0f)}},
                                      InterpolationType::Smooth,
                                      1.0f, true};
    Animation<float> rotate = {{{0.0f, 0.0f},
                                {1.0f, 360.0f}},
                               InterpolationType::Linear,
                               0.015f, true};
    dataBlock = RewardAnimatorData{translate, rotate};
},

// RewardAnimator::OnUpdate
[](Entity entity, std::any& dataBlock)
{
    auto* transform = entity.FindComponent<Transform>();
    if (!transform)
    {
        return;
    }

    RewardAnimatorData& data = std::any_cast<RewardAnimatorData&>(dataBlock);
    transform->transform = glm::translate(glm::mat4(1.0f), data.translateAnimation.Evaluate(Time::GetDeltaTime())) *
                           glm::rotate(glm::mat4(1.0f), data.rotateAnimation.Evaluate(Time::GetDeltaTime()), glm::vec3(0, 1, 0));
},

// RewardAnimator::OnEvent
nullptr,

// RewardAnimator::OnDestroy
nullptr);

} 


Scriptable CreateHealLogic(const Entity& entity)
{
    return Scriptable(
        "MonsterLogic",
        entity,

// HealLogic::OnCreate
[](Entity entity, std::any& dataBlock)
{
    dataBlock = std::make_any<HealData>();
},

// HealLogic::OnUpdate
nullptr,

// HealLogic::OnEvent
[](Event* event, Entity entity, std::any& dataBlock) {
    switch (event->GetCategory())
    {
        case EventCategory::Game:
        {
            switch (event->GetType())    
            {
                case TriggerEnterEvent::TypeId:
                {
                    // GetPlayer().Heal(data.healing)
                    entity.Remove();
                }
            }
            break;
        }
    }
},

// HealLogic::OnDestroy
nullptr);
} 


Scriptable CreateWeaponLogic(const Entity& entity)
{
    return Scriptable(
        "WeaponLogic",
        entity,

// WeaponLogic::OnCreate
[](Entity entity, std::any& dataBlock)
{
},

// WeaponLogic::OnUpdate
nullptr,

// WeaponLogic::OnEvent
[](Event* event, Entity entity, std::any& dataBlock) {
    switch (event->GetCategory())
    {
        case EventCategory::Game:
        {
            switch (event->GetType())    
            {
                case TriggerEnterEvent::TypeId:
                {
                    // GetPlayer().ChangeWeapon(entity)
                    entity.Remove();
                }
            }
            break;
        }
    }
},

// WeaponLogic::OnDestroy
nullptr);
} 

} // Namespace Components
