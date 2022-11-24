#include "Entity.h"


const std::string& Entity::GetName() const
{
    return m_scene->GetEntityName(m_id);
}

Entity Entity::GetParent() const
{
    return Entity(m_scene->GetEntityParent(m_id), m_scene);
}

std::vector<Entity> Entity::GetChildren() const 
{
    const auto& hierarchy = GetComponent<HierarchyComponent>();
    if (!hierarchy.firstChild)
    {
        return {};
    }

    Entity child = Entity(hierarchy.firstChild, m_scene);

    std::vector<Entity> children;
    children.push_back(child);
    for (size_t i=0 ; i < hierarchy.childCount - 1 ; i++)
    {
        child = Entity(child.GetComponent<HierarchyComponent>().nextSibling, m_scene);
        children.push_back(child);
    }
    
    return children;
}

Entity Entity::FindChild(const std::string& name) const
{
    const auto& hierarchy = GetComponent<HierarchyComponent>();
    if (hierarchy.firstChild)
    {
        Entity child = Entity(hierarchy.firstChild, m_scene);
        if (child.GetName() == name)
        {
            return child;
        }

        for (size_t i=0 ; i < hierarchy.childCount - 1 ; i++)
        {
            child = Entity(child.GetComponent<HierarchyComponent>().nextSibling, m_scene);
            if (child.GetName() == name)
            {
                return child;
            }
        }
    }
    
    return {};
}

Entity Entity::AddChild(const std::string& name) const
{
    return m_scene->CreateEntity(name, *this);
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
