#ifndef ANIMATION_H
#define ANIMATION_H

#include "Core/Application.h"

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
    float currentTime = 0.0f;
    bool ended = true;

    void Start()
    {
        currentTime = 0.0f;
        ended = false;
    }

    T Evaluate(const float& deltaTime)
    {
        currentTime += deltaTime * speed;
        float prevTime = 0;
        T prevValue;

        // Could be optimized by storing an iterator instead of looping over the keys at each evaluation
        for (const auto [keyTime, keyValue] : keyframes)
        {
            if (keyTime == currentTime)
            {
                return keyValue;
            }
            
            if (keyTime > currentTime)
            {
                switch (interpolation)
                {
                    case InterpolationType::Step:
                    {
                        return prevValue;
                    }

                    case InterpolationType::Linear:
                    {
                        float weight = (currentTime - prevTime) / (keyTime - prevTime);
                        return prevValue * (1.0f - weight) + keyValue * weight;
                    }

                    case InterpolationType::Smooth:
                    {
                        float weight = (currentTime - prevTime) / (keyTime - prevTime);
                        weight = weight * weight * (3 - 2 * weight);  // cubic hermite interpolation
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

#endif  // ANIMATION_H