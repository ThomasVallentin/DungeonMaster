#ifndef LIGHTCOMPONENTS_H
#define LIGHTCOMPONENTS_H

#include <glm/glm.hpp>


struct PointLight
{
    PointLight() = default;
    PointLight(const glm::vec3& color, const float& intensity=1.0f, const float& decay=2.0f) :
            color(color), intensity(intensity), decay(decay) {}

    glm::vec3 color = {1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;
    float decay = 2.0f;
};


#endif  // LIGHTCOMPONENTS_H
