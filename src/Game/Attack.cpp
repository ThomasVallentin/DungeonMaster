#include "Attack.h"

#include "GameEvents.h"

#include "Scene/Components/Basics.h"

#include "Scripting/Engine.h"

#include <glm/gtx/norm.hpp>


void PerformAttack(const Attack& attack, const std::vector<Entity>& targets)
{
    glm::vec2 pos = {round(attack.source.x), round(attack.source.z)};
    glm::vec2 viewDir = glm::normalize(glm::vec2(attack.direction.x, attack.direction.z));

    for (auto& target : targets)
    {
        glm::mat4 victimWorldMatrix = Components::Transform::ComputeWorldMatrix(target);
        glm::vec2 targetPos = {round(victimWorldMatrix[3].x), round(victimWorldMatrix[3].z)};
        glm::vec2 toTarget = targetPos - pos;

        // Victim is too far away
        if (glm::length2(toTarget) > (attack.range * attack.range))
        {
            return;
        }

        glm::vec2 toTargetDir = glm::normalize(toTarget);

        // Victim is not in the angle of view
        if (std::max(glm::dot(viewDir, toTargetDir), 0.0f) < (std::cos(attack.angleOfEffect)))
        {
            return;
        }

        AttackEvent event(target, attack);
        Scripting::Engine::Get().EmitGameEvent(&event);
    }
}
