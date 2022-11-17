#include "Entity.h"


const std::string& Entity::GetName() const
{
    return m_scene->GetEntityName(m_id);
}

Entity Entity::GetParent() const
{
    return Entity(m_scene->GetEntityParent(m_id), m_scene);
}

void Entity::Remove()
{
    m_scene->RemoveEntity(*this);
    m_id = 0;
}

bool Entity::IsValid() const
{
    return m_id && m_scene;
}

bool Entity::IsRoot() const
{
    return m_id == m_scene->m_rootId;
}
