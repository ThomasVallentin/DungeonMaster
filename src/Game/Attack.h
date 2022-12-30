#ifndef ATTACK_H
#define ATTACK_H

#include "Scene/Entity.h"

#include <glm/glm.hpp>

#include <vector>

struct Attack 
{
    const float damage;

    // These attributes should be replaced by a collider spawned for a single frame which would collide with an hitbox through the trigger system
    const glm::vec3 source;
    const glm::vec3 direction;
    const float range;
    const float angleOfEffect;
};

void PerformAttack(const Attack& attack, const std::vector<Entity>& victims);


#endif // ATTACK_H