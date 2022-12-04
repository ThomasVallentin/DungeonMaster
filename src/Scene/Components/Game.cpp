#include "Game.h"

#include "Core/Event.h"
#include "Core/Application.h"
#include "Core/Animation.h"

#include <GLFW/glfw3.h>

#include <map>


namespace Components {


struct CharacterControllerData
{
    Animation<glm::mat4> moveAnimation;
    Animation<glm::mat4> attackAnimation;
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
    CharacterControllerData& data = std::any_cast<CharacterControllerData&>(dataBlock);
    if (!data.moveAnimation.ended)
    {
        auto* transform = entity.FindComponent<Transform>();
        if (transform)
        {
            transform->transform = data.moveAnimation.Evaluate(Application::Get().GetCurrentTime());
        }
    }

    if (!data.attackAnimation.ended)
    {
        auto* childTransform = entity.FindChild("Weapon").FindComponent<Transform>();
        if (childTransform)
        {
            childTransform->transform = data.attackAnimation.Evaluate(Application::Get().GetCurrentTime());
        }
    }
},

// CharacterController::OnEvent
[](Event* event, const Entity& entity, std::any& dataBlock)
{
    CharacterControllerData& data = std::any_cast<CharacterControllerData&>(dataBlock);

    switch(event->GetType())
    {
        case EventType::KeyPressed:
        {
            if (!data.moveAnimation.ended)
            {
                return;
            }

            KeyPressedEvent* keyPressedEvent = dynamic_cast<KeyPressedEvent*>(event);
            auto* transform = entity.FindComponent<Transform>();
            if (!transform)
            {
                return;
            }

            switch (keyPressedEvent->GetKey())
            {
                case GLFW_KEY_UP:
                {
                    LOG_DEBUG("CharacterController : KEY UP !!!");
                    data.moveAnimation = {{{0.0f, transform->transform},
                                           {1.0f, glm::translate(transform->transform, glm::vec3(0, 0, -1))}},
                                          InterpolationType::Smooth,
                                          2.5f};
                    data.moveAnimation.Start();
                    break;
                }
                case GLFW_KEY_DOWN:
                {
                    LOG_DEBUG("CharacterController : KEY DOWN !!!");
                    data.moveAnimation = {{{0.0f, transform->transform},
                                           {1.0f, glm::translate(transform->transform, glm::vec3(0, 0, 1))}},
                                          InterpolationType::Smooth,
                                          2.5f};
                    data.moveAnimation.Start();
                    break;
                }
                case GLFW_KEY_LEFT:
                {
                    LOG_DEBUG("CharacterController : KEY LEFT !!!");
                    data.moveAnimation = {{{0.0f, transform->transform},
                                           {1.0f, glm::rotate(transform->transform, (float)M_PI_2, glm::vec3(0, 1, 0))}},
                                          InterpolationType::Smooth,
                                          2.5f};
                    data.moveAnimation.Start();
                    break;
                }
                case GLFW_KEY_RIGHT:
                {
                    LOG_DEBUG("CharacterController : KEY RIGHT !!!");
                    data.moveAnimation = {{{0.0f, transform->transform},
                                           {1.0f, glm::rotate(transform->transform, -(float)M_PI_2, glm::vec3(0, 1, 0))}},
                                          InterpolationType::Smooth,
                                          2.5f};
                    data.moveAnimation.Start();
                    break;
                }
            }

            break;
        }

        case EventType::MouseButtonPressed:
        {
            if (!data.attackAnimation.ended)
            {
                return;
            }

            MouseButtonPressedEvent* mouseEvent = dynamic_cast<MouseButtonPressedEvent*>(event);
            auto* transform = entity.FindComponent<Transform>();
            if (!transform)
            {
                return;
            }

            switch (mouseEvent->GetButton())
            {
                case GLFW_MOUSE_BUTTON_LEFT:
                {
                    LOG_INFO("SCHLACK !!!");
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
                            1.5f
                        };
                        data.attackAnimation.Start();
                    }
                    break;
                }
            }
        }
    }
},
nullptr);

} 

// Script CreateNavAgent(const Entity& entity)
// {
//     return Script(
//         "NavAgent",
//         entity,

// // NavAgent::OnCreate
// [](const Entity& entity, std::any& dataBlock)
// {
//     dataBlock = std::make_any<CharacterControllerData>();
// },

// // NavAgent::OnUpdate
// [](const Entity& entity, std::any& dataBlock)
// {
//     CharacterControllerData& data = std::any_cast<CharacterControllerData&>(dataBlock);
//     if (!data.moveAnimation.ended)
//     {
//         auto* transform = entity.FindComponent<Transform>();
//         if (transform)
//         {
//             transform->transform = data.moveAnimation.Evaluate(Application::Get().GetCurrentTime());
//         }
//     }

//     if (!data.attackAnimation.ended)
//     {
//         auto* childTransform = entity.FindChild("Weapon").FindComponent<Transform>();
//         if (childTransform)
//         {
//             childTransform->transform = data.attackAnimation.Evaluate(Application::Get().GetCurrentTime());
//         }
//     }
// },

// // NavAgent::OnEvent
// [](Event* event, const Entity& entity, std::any& dataBlock)
// {

// });
// }


} // Namespace Components
