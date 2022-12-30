#include "Attack.h"

#include "GameEvents.h"

#include "Scene/Components/Basics.h"

#include <glm/gtx/norm.hpp>


void PerformAttack(const Attack& attack, const std::vector<Entity>& victims)
{
    glm::vec2 pos = {round(attack.source.x), round(attack.source.z)};
    glm::vec2 viewDir = glm::normalize(glm::vec2(-attack.direction.x, attack.direction.z));

    for (auto& victim : victims)
    {
        glm::mat4 victimWorldMatrix = Components::Transform::ComputeWorldMatrix(victim);
        // LOG_INFO("victim pos : %s", glm::to_string(victimWorldMatrix).c_str());
        glm::vec2 targetPos = {round(victimWorldMatrix[3].x), round(victimWorldMatrix[3].z)};
        glm::vec2 toTarget = targetPos - pos;

        // Victim is too far away
        // LOG_INFO("victim distance %f %f", glm::length2(toTarget), (attack.range * attack.range));
        if (glm::length2(toTarget) > (attack.range * attack.range))
        {
            return;
        }

        glm::vec2 toTargetDir = glm::normalize(toTarget);

        // Victim is not in the angle of view
        // LOG_INFO("victim dir %f %f", std::max(glm::dot(viewDir, toTargetDir), 0.0f), (std::cos(glm::radians(attack.angleOfEffect))));
        if (std::max(glm::dot(viewDir, toTargetDir), 0.0f) < (std::cos(attack.angleOfEffect)))
        {
            return;
        }

        AttackEvent event(victim, attack);
        Scripting::Engine::Get().EmitGameEvent(&event);
    }
}