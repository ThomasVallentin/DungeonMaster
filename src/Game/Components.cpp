#include "Components.h"

#include "Navigation/Components.h"
#include "Navigation/Engine.h"

#include "Core/Event.h"
#include "Core/Inputs.h"
#include "Core/Time.h"
#include "Core/Animation.h"

namespace Components {


// == Character Controller ==

struct CharacterControllerData
{
    Animation<glm::mat4> moveAnimation;
    Animation<glm::mat4> attackAnimation;
    float speed = 2.0f;
    Navigation::CellFilters navFilter = Navigation::CellFilters::Default;
};


Scriptable CreateCharacterController(const Entity& entity)
{
    return Scriptable(
        "CharacterController",
        entity,

// CharacterController::OnCreate
[](const Entity& entity, std::any& dataBlock)
{
    dataBlock = std::make_any<CharacterControllerData>();
},

// CharacterController::OnUpdate
[](const Entity& entity, std::any& dataBlock)
{
    auto* transform = entity.FindComponent<Transform>();
    if (!transform)
    {
        return;
    }

    CharacterControllerData& data = std::any_cast<CharacterControllerData&>(dataBlock);
    Navigation::Engine& navEngine = Navigation::Engine::Get();

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
        auto* childTransform = entity.FindChild("Weapon").FindComponent<Transform>();
        if (childTransform)
        {
            childTransform->transform = data.attackAnimation.Evaluate(Time::GetDeltaTime());
        }
        shouldSampleInput = false;
    }

    if (!shouldSampleInput)
    {
        return;
    }

    // Sample keyboard inputs
    if (Inputs::IsKeyPressed(KeyCode::Up))
    {
        glm::mat4 nextTransform = glm::translate(transform->transform, glm::vec3(0, 0, -1.0f));
        glm::vec2 nextCell = glm::vec2(nextTransform[3].x, -nextTransform[3].z);
        if (navEngine.IsWalkableCell(nextCell, data.navFilter))
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
        glm::vec2 nextCell = glm::vec2(nextTransform[3].x, -nextTransform[3].z);
        if (navEngine.IsWalkableCell(nextCell, data.navFilter))
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
                    {0.2f, glm::rotate(childTransform->transform, -(float)M_PI / 8.0f, glm::vec3(0, 1, 0))},
                    {0.4f, glm::rotate(childTransform->transform, (float)M_PI / 4.0f, glm::vec3(0, 1, 0))},
                    {1.0f, childTransform->transform}
                },
                InterpolationType::Smooth, 
                data.speed
            };
            data.attackAnimation.Start();
        }
    }
},

// CharacterController::OnEvent
[](Event* event, const Entity& entity, std::any& dataBlock) {},
nullptr);

} 

// == Moster Logic ==

struct MonsterLogicData
{
    float speed = 2.0f;
    float angleOfView = 45.0f;
    Navigation::CellFilters navFilter = Navigation::CellFilters::Default;
    Entity target;
};


Scriptable CreateMonsterLogic(const Entity& entity)
{
    return Scriptable(
        "MonsterLogic",
        entity,

// CharacterController::OnCreate
[](const Entity& entity, std::any& dataBlock)
{
    dataBlock = std::make_any<MonsterLogicData>();
},

// CharacterController::OnUpdate
[](const Entity& entity, std::any& dataBlock)
{
    NavAgent* navAgent = entity.FindComponent<NavAgent>();
    if (!navAgent || navAgent->GetAgent()->IsMoving())
    {
        return;
    }

    MonsterLogicData& data = std::any_cast<MonsterLogicData&>(dataBlock);
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

    glm::vec2 pos = {transform->transform[3].x, transform->transform[3].z};
    glm::vec2 targetPos = {targetTransform->transform[3].x, targetTransform->transform[3].z};
    glm::vec2 toTarget = targetPos - pos;

    glm::vec2 viewDir = glm::normalize(glm::vec2(transform->transform[2].x, transform->transform[2].z));
    glm::vec2 targetDir = glm::normalize(toTarget);

    if (std::max(glm::dot(viewDir, targetDir), 0.0f) > (std::cos(glm::radians(data.angleOfView * 0.5f))))
    {
        LOG_INFO("In view !");
    }

    Navigation::Engine& navEngine = Navigation::Engine::Get();





},

// CharacterController::OnEvent
[](Event* event, const Entity& entity, std::any& dataBlock) {},
nullptr);

} 


} // Namespace Components
