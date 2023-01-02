#include "Components.h"

#include "GameEvents.h"
#include "GameManager.h"

#include "Navigation/Components.h"

#include "Scripting/Engine.h"
#include "Scripting/Trigger.h"

#include "Renderer/Renderer.h"

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
    CharacterControllerData& data = std::any_cast<CharacterControllerData&>(dataBlock);
    if (!data.haloEffectAnimation.ended)
    {
        Renderer::Get().GetPostProcessShader()->SetVec4("uHaloColor", data.haloEffectAnimation.Evaluate(Time::GetDeltaTime()));
    }
    else 
    {
        Renderer::Get().GetPostProcessShader()->SetVec4("uHaloColor", glm::vec4(0.0f));
    }

    auto& transform = entity.GetComponent<Transform>();
    bool shouldSampleInput = true;

    // The character is currently moving, evaluate the move animation
    if (!data.moveAnimation.ended)
    {
        transform.transform = data.moveAnimation.Evaluate(Time::GetDeltaTime());
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
        glm::mat4 nextTransform = glm::translate(transform.transform, glm::vec3(0, 0, -1.0f));
        glm::vec2 nextCell = glm::vec2(nextTransform[3].x, nextTransform[3].z);
        if (navEngine.CellIsEmpty(nextCell, data.navFilter) && !navEngine.CellContainsAgent(nextCell))
        {
            data.moveAnimation = {{{0.0f, transform.transform},
                                   {1.0f, nextTransform}},
                                  InterpolationType::Smooth,
                                  data.speed};
            data.moveAnimation.Start();
        }
    }
    else if (Inputs::IsKeyPressed(KeyCode::Down))
    {
        glm::mat4 nextTransform = glm::translate(transform.transform, glm::vec3(0, 0, 1.0f));
        glm::vec2 nextCell = glm::vec2(nextTransform[3].x, nextTransform[3].z);
        if (navEngine.CellIsEmpty(nextCell, data.navFilter) && !navEngine.CellContainsAgent(nextCell))
        {
            data.moveAnimation = {{{0.0f, transform.transform},
                                   {1.0f, nextTransform}},
                                  InterpolationType::Smooth,
                                  data.speed};
            data.moveAnimation.Start();
        }
    }
    else if (Inputs::IsKeyPressed(KeyCode::Left))
    {
        data.moveAnimation = {{{0.0f, transform.transform},
                               {1.0f, glm::rotate(transform.transform, (float)M_PI_2, glm::vec3(0, 1, 0))}},
                              InterpolationType::Smooth,
                              data.speed};
        data.moveAnimation.Start();
    }
    else if (Inputs::IsKeyPressed(KeyCode::Right))
    {
        data.moveAnimation = {{{0.0f, transform.transform},
                               {1.0f, glm::rotate(transform.transform, -(float)M_PI_2, glm::vec3(0, 1, 0))}},
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
            auto* attackEvent = dynamic_cast<AttackEvent*>(event);
            auto& character = entity.GetComponent<CharacterData>();
            character.InflictDamage(attackEvent->GetAttack().damage);

            CharacterControllerData& data = std::any_cast<CharacterControllerData&>(dataBlock);
            data.haloEffectAnimation = {{{0.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.2f)},
                                         {1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.0)}},
                                        InterpolationType::Smooth,
                                        1.0f, false};
            data.haloEffectAnimation.Start();

            LOG_INFO("You've been hit ! Only %f health left...", character.health);
            if (!character.IsAlive())
            {
                GameManager::Get().ShowGameOverScreen();
            }
            break;
        }

        case HealEvent::TypeId:
        {
            auto* healEvent = dynamic_cast<HealEvent*>(event);
            auto* character = entity.FindComponent<CharacterData>();
            character->Heal(healEvent->GetHealing());

            CharacterControllerData& data = std::any_cast<CharacterControllerData&>(dataBlock);
            data.haloEffectAnimation = {{{0.0f, glm::vec4(0.25f, 0.9f, 0.13f, 0.15f)},
                                         {1.0f, glm::vec4(0.25f, 0.9f, 0.13f, 0.0)}},
                                        InterpolationType::Smooth,
                                        1.0f, false};
            data.haloEffectAnimation.Start();

            break;
        }

        case PickupWeaponEvent::TypeId:
        {
            auto* weaponEvent = dynamic_cast<PickupWeaponEvent*>(event);
            const WeaponData& newWeaponData = weaponEvent->GetWeapon();
            LOG_INFO("%s", weaponEvent->GetWeapon().modelIdentifier.c_str());

            Entity weapon = entity.FindChild("Weapon");
            WeaponData& weaponData = weapon.GetComponent<WeaponData>();
            if (newWeaponData.GetDPS() > weaponData.GetDPS())
            {
                weaponData = newWeaponData;

                // Switch weapon models
                weapon.FindChild("model").Remove();
                auto model = ResourceManager::LoadModel(newWeaponData.modelIdentifier);
                weapon.GetScene()->CopyEntity(model.Get()->GetRootEntity(), "model", weapon);
            }
            break;
        }
    }
},

// CharacterController::OnDestroy
nullptr

);

} 


// == Monster Logic ==

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
    if (!data.target)
    {
        return;
    }

    glm::mat4 worldMatrix = Transform::ComputeWorldMatrix(entity);
    glm::mat4 targetWorldMatrix = Transform::ComputeWorldMatrix(data.target);

    glm::vec2 pos = {round(worldMatrix[3].x), round(worldMatrix[3].z)};
    glm::vec2 targetPos = {round(targetWorldMatrix[3].x), round(targetWorldMatrix[3].z)};
    glm::vec2 toTarget = targetPos - pos;
    float targetDistance = glm::length2(toTarget);

    // Target is too far away
    if (targetDistance > (data.viewDistance * data.viewDistance))
    {
        return;
    }

    glm::vec2 viewDir = glm::normalize(glm::vec2(worldMatrix[2].x, worldMatrix[2].z));
    glm::vec2 toTargetDir = glm::normalize(toTarget);

    // Target is not in the angle of view
    if (std::max(glm::dot(viewDir, toTargetDir), 0.0f) < (std::cos(glm::radians(data.angleOfView * 0.5f))))
    {
        return;
    }
    
    // Something is blocking the view (a wall for example)
    Navigation::Engine& navEngine = Navigation::Engine::Get();
    if (!navEngine.CanSeeCell(pos, targetPos))
    {
        return;
    }

    navAgent->GetAgent()->SetDestination(glm::vec3(targetPos.x, 0.0f, targetPos.y));

    if (navAgent->GetAgent()->IsMoving())
    {
        return;
    }

    if (data.attackDelay < (1.0 / data.attackSpeed))
    {
        data.attackDelay += Time::GetDeltaTime();
    }
    else
    {
        PerformAttack(Attack{data.strength,
                             worldMatrix[3],
                             worldMatrix[2],
                             1.001,  // Attack range is a little bit over 1.0 to ensure the attack doesn't get skiped due to some imprecision 
                             data.angleOfView}, 
                      {data.target});
        data.attackDelay = 0.0;
    }
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
            LOG_INFO("You hit %s ! only %f health left...", entity.GetName().c_str(), characterData.health);
            if (!characterData.IsAlive())
            {
                entity.Remove();
                return;
            }
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

// == Rewards ==

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

// == Heal Logic ==

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
                    TriggerEnterEvent* triggerEvent = dynamic_cast<TriggerEnterEvent*>(event);
                    HealData& data = std::any_cast<HealData&>(dataBlock);

                    HealEvent healEvent(triggerEvent->GetSource(), data.healing);
                    Scripting::Engine::Get().EmitGameEvent(&healEvent);
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

// == Weapon Logic ==

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
                    TriggerEnterEvent* triggerEvent = dynamic_cast<TriggerEnterEvent*>(event);
                    WeaponData& data = entity.GetComponent<WeaponData>();

                    PickupWeaponEvent weaponEvent(triggerEvent->GetSource(), data);
                    Scripting::Engine::Get().EmitGameEvent(&weaponEvent);
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


// == Door Logic ==

Scriptable CreateDoorLogic(const Entity& entity)
{
    return Scriptable(
        "DoorLogic",
        entity,

// DoorLogic::OnCreate
[](Entity entity, std::any& dataBlock)
{
    dataBlock = std::make_any<DoorData>();
},

// DoorLogic::OnUpdate
[](Entity entity, std::any& dataBlock)
{
    DoorData& data = std::any_cast<DoorData&>(dataBlock);
    if (data.openAnimation.ended)
    {
        return;
    }

    Transform* transform = entity.FindChild("model").FindComponent<Transform>();
    if (!transform)
    {
        return;
    }

    transform->transform = data.openAnimation.Evaluate(Time::GetDeltaTime());
    if (data.openAnimation.ended)
    {
        data.opened = true;
        glm::mat4 worldMatrix = Transform::ComputeWorldMatrix(entity);
        Navigation::Engine::Get().SetCell(round(worldMatrix[3].x), round(worldMatrix[3].z), Navigation::CellFilters::Floor);
    }
},

// DoorLogic::OnEvent
[](Event* event, Entity entity, std::any& dataBlock)
{
    switch (event->GetType())
    {
        case TriggerEnterEvent::TypeId:
        case TriggerStayEvent::TypeId:
        {
            if (Inputs::IsKeyPressed(KeyCode::Enter) || Inputs::IsKeyPressed(KeyCode::KeyPad_Enter))
            {
                DoorData& data = std::any_cast<DoorData&>(dataBlock);
                if (data.opened)
                {
                    return;
                }

                data.openAnimation = {{{0.0f, glm::mat4(1.0f)},
                                       {1.0f, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.9f, 0.0f))}
                                      },
                                      InterpolationType::Smooth,
                                      1.5f, false};
                data.openAnimation.Start();
            }
        }
    };
},

// DoorLogic::OnDestroy
nullptr);
}


// == Title Screen Logic ==

Scriptable CreateTitleScreenLogic(const Entity& entity)
{
    return Scriptable(
        "TitleScreen",
        entity,

// TitleScreenLogic::OnCreate
nullptr,

// TitleScreenLogic::OnUpdate
[](Entity entity, std::any& dataBlock)
{
    if (Inputs::IsKeyPressed(KeyCode::Enter) || Inputs::IsKeyPressed(KeyCode::KeyPad_Enter))
    {
        GameManager::Get().StartGame();
    }
    else if (Inputs::IsKeyPressed(KeyCode::Escape))
    {
        Application::Get().Stop();
    }
},

// TitleScreenLogic::OnEvent
nullptr,

// TitleScreenLogic::OnDestroy
nullptr);

}


// == Game Over Logic ==

Scriptable CreateGameOverLogic(const Entity& entity)
{
    return Scriptable(
        "GameOverLogic",
        entity,

// GameOverLogic::OnCreate
nullptr,

// GameOverLogic::OnUpdate
[](Entity entity, std::any& dataBlock)
{
    if (Inputs::IsKeyPressed(KeyCode::Enter) || Inputs::IsKeyPressed(KeyCode::Backspace))
    {
        GameManager::Get().RestartGame();
    }
},

// GameOverLogic::OnEvent
nullptr,

// GameOverLogic::OnDestroy
nullptr);

}


// == Exit Logic ==

Scriptable CreateExitLogic(const Entity& entity)
{
    return Scriptable(
        "TitleScreen",
        entity,

// ExitLogic::OnCreate
nullptr,

// ExitLogic::OnUpdate
nullptr,

// ExitLogic::OnEvent
[](Event* event, Entity entity, std::any& dataBlock)
{
    switch (event->GetType())
    {
        case TriggerEnterEvent::TypeId:
        {
            LOG_INFO("END !");
            GameManager::Get().ShowEndScreen();
        }
    };
},

// ExitLogic::OnDestroy
nullptr);

}


// == End Screen Logic ==

Scriptable CreateEndScreenLogic(const Entity& entity)
{
    return Scriptable(
        "EndScreen",
        entity,

// EndScreenLogic::OnCreate
nullptr,

// EndScreenLogic::OnUpdate
[](Entity entity, std::any& dataBlock)
{
    if (Inputs::IsKeyPressed(KeyCode::Enter) || Inputs::IsKeyPressed(KeyCode::KeyPad_Enter))
    {
        Application::Get().Stop();
    }
},

// EndScreenLogic::OnEvent
nullptr,

// EndScreenLogic::OnDestroy
nullptr);

}


} // Namespace Components
