#include "Components.h"

#include "Navigation/Components.h"

#include "Core/Event.h"
#include "Core/Inputs.h"
#include "Core/Time.h"
#include "Core/Animation.h"

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
    
    Navigation::Engine& navEngine = Navigation::Engine::Get();

    // Sample keyboard inputs
    if (Inputs::IsKeyPressed(KeyCode::Up))
    {
        glm::mat4 nextTransform = glm::translate(transform->transform, glm::vec3(0, 0, -1.0f));
        glm::vec2 nextCell = glm::vec2(nextTransform[3].x, nextTransform[3].z);
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
        glm::vec2 nextCell = glm::vec2(nextTransform[3].x, nextTransform[3].z);
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
                    {0.2f, glm::rotate(childTransform->transform, -(float)M_PI / 8.0f, glm::vec3(0, 0, 1))},
                    {0.4f, glm::rotate(childTransform->transform, (float)M_PI / 4.0f, glm::vec3(0, 0, 1))},
                    {1.0f, childTransform->transform}
                },
                InterpolationType::Smooth, 
                1.5f
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

Scriptable CreateMonsterLogic(const Entity& entity)
{
    return Scriptable(
        "MonsterLogic",
        entity,

// MonsterLogic::OnCreate
[](const Entity& entity, std::any& dataBlock)
{
    dataBlock = std::make_any<MonsterLogicData>();
},

// MonsterLogic::OnUpdate
[](const Entity& entity, std::any& dataBlock)
{
    NavAgent* navAgent = entity.FindComponent<NavAgent>();
    auto agent = navAgent->GetAgent();
    if (!navAgent)
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

    glm::vec2 pos = {round(transform->transform[3].x), round(transform->transform[3].z)};
    LOG_INFO("%s", glm::to_string(pos).c_str());
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
                if (!navEngine.IsWalkableCell(pos, data.navFilter))
                    return;
                pos.y += ySign;
                e = ySign - e;
            }

            if (!navEngine.IsWalkableCell(pos, data.navFilter))
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
                if (!navEngine.IsWalkableCell(pos, data.navFilter))
                    return;
                pos.x += xSign;
                e = xSign - e;
            }

            if (!navEngine.IsWalkableCell(pos, data.navFilter))
                return;
            pos.y++;
            e -= toTargetDir.x;
        }
    }

    navAgent->GetAgent()->SetDestination(glm::vec3(targetPos.x, 0.0f, targetPos.y));
},

// MonsterLogic::OnEvent
[](Event* event, const Entity& entity, std::any& dataBlock) {},
nullptr);

} 


} // Namespace Components
