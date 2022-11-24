#ifndef GAMECOMPONENTS_H
#define GAMECOMPONENTS_H

#include "Basics.h"

#include "Core/Event.h"
#include "Core/Application.h"

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

    Script CreateCharacterController(const Entity& entity);

} // Namespace Components


#endif // GAMECOMPONENTS_H