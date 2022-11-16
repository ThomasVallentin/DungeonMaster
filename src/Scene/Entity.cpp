#include "Entity.h"


bool Entity::IsValid() const
{
    return m_id && m_scene;
}

const std::string& Entity::GetName() 
{
    return GetComponent<BaseComponent>().name;
}

void Entity::Remove()
{
    m_scene->RemoveEntity(*this);
    m_id = 0;
}
