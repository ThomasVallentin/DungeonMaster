#include "Trigger.h"
#include "Engine.h"

#include "Scene/Components/Basics.h"
#include "Game/GameEvents.h"

#include <glm/gtx/norm.hpp>

namespace Components {


void Trigger::OnUpdate()
{
    Entity entity = GetEntity();
    Entity target = GetTarget();
    if (!entity || !target)
        return;

    glm::mat4 matrix = Transform::ComputeWorldMatrix(entity);
    glm::mat4 targetMatrix = Transform::ComputeWorldMatrix(target);

    if (glm::distance2(glm::vec3(matrix[3]), glm::vec3(targetMatrix[3])) <= (m_radius * m_radius))
    {
        if (!m_isInside)
        {
            m_isInside = true;
            TriggerEnterEvent event(entity, target);
            Scripting::Engine::Get().EmitGameEvent(&event);
        }
        else 
        {
            TriggerStayEvent event(entity, target);
            Scripting::Engine::Get().EmitGameEvent(&event);
        }
    }
    else if (m_isInside)
    {
        m_isInside = false;
        TriggerExitEvent event(entity, target);
        Scripting::Engine::Get().EmitGameEvent(&event);
    }

}


} // Namespace Components
