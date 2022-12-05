#include "Basics.h"


namespace Components {


// == Transform ==

glm::mat4 Transform::ComputeWorldMatrix(const Entity& entity)
{
    glm::mat4 result = glm::mat4(1.0f);

    Entity parent = entity;
    while (parent)
    {
        if (auto* transform = parent.FindComponent<Components::Transform>())
        {
            result = transform->transform * result;
        }
        parent = parent.GetParent();
    }

    return result;
}

}  // Namespace Components