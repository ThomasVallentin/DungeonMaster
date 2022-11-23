#ifndef GAMECOMPONENTS_H
#define GAMECOMPONENTS_H

#include "Basics.h"

#include "Core/Event.h"

#include <GLFW/glfw3.h>

#include <map>

// The application doesn't allow us to load scripts yet, which means
// that they have to be hard coded somewhere. This file contains them all.

enum InterpolationType
{
    Step = 0,
    Linear,
    Smooth
};


template <typename T>
struct Animation
{
    std::map<float, const T> keyframes;
    InterpolationType interpolation = Linear;
    float speed = 1.0f;
    float startTime = 0.0f;
    bool ended = true;

    void Start()
    {
        startTime = Application::Get().GetCurrentTime();
        ended = false;
    }

    T Evaluate(const float& time)
    {
        float scaledTime = (time - startTime) * speed;
        T prevValue;
        float prevTime = 0;
        for (const auto [keyTime, keyValue] : keyframes)
        {
            if (keyTime == scaledTime)
            {
                return keyValue;
            }
            
            if (keyTime > scaledTime)
            {
                switch (interpolation)
                {
                    case InterpolationType::Step:
                    {
                        return prevValue;
                    }

                    case InterpolationType::Linear:
                    {
                        float weight = (scaledTime - prevTime) / (keyTime - prevTime);
                        return prevValue * (1.0f - weight) + keyValue * weight;
                    }

                    case InterpolationType::Smooth:
                    {
                        float weight = (scaledTime - prevTime) / (keyTime - prevTime);
                        weight = weight * weight * (3 - 2 * weight);
                        return prevValue * (1.0f - weight) + keyValue * weight;
                    }
                }
            }

            prevTime = keyTime;
            prevValue = keyValue;
        }
        
        ended = true;
        return prevValue;
    }
};


namespace Components {


struct CharacterControllerData
{
    Animation<glm::vec3> animation;
};


static Script CreateCharacterController(const Entity& entity)
{
    return Script(
        "CharacterController",
        entity,

        // OnCreate
        [](const Entity& entity, std::any& dataBlock)
        {
            dataBlock = std::make_any<CharacterControllerData>();
            LOG_INFO("CREATE !!!");
        },

        // OnUpdate
        [](const Entity& entity, std::any& dataBlock)
        {
            CharacterControllerData& data = std::any_cast<CharacterControllerData&>(dataBlock);
            if (!data.animation.ended)
            {
                auto* transform = entity.FindComponent<Transform>();
                if (transform)
                {
                    glm::vec3 anim = data.animation.Evaluate(Application::Get().GetCurrentTime());
                    transform->transform[3] = glm::vec4((anim), 1.0f);
                    LOG_INFO("Anim : %f : %f, %f, %f", 
                             Application::Get().GetCurrentTime() - data.animation.startTime,
                             anim.x, anim.y, anim.z);
                }
            }
        },

        // OnEvent
        [](Event* event, const Entity& entity, std::any& dataBlock)
        {
            CharacterControllerData& data = std::any_cast<CharacterControllerData&>(dataBlock);
            if (!data.animation.ended)
            {
                return;
            }

            switch(event->GetType())
            {
                case EventType::KeyPressed:
                {
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
                            LOG_INFO("KEY UP !!!");
                            data.animation = {{{0.0f, glm::vec3(transform->transform[3])},
                                               {1.0f, glm::vec3(transform->transform[3]) + glm::vec3(0, 0, 1)}},
                                              InterpolationType::Smooth,
                                              2.5f};
                            data.animation.Start();
                            break;
                        }
                    }

                    break;
                }
            }
        }
    );
} 

}


#endif // GAMECOMPONENTS_H