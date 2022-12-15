#include "Trigger.h"

#include "Scene/Components/Basics.h"

#include "Core/Application.h"

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
            Application::Get().EmitEvent(&event);
        }
        else 
        {
            TriggerStayEvent event(entity, target);
            Application::Get().EmitEvent(&event);
        }
    }
    else if (m_isInside)
    {
        m_isInside = false;
        TriggerExitEvent event(entity, target);
        Application::Get().EmitEvent(&event);
    }

}


} // Namespace Components
