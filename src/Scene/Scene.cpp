#include "Scene.h"

#include "Entity.h"
#include <iostream>


Scene::Scene()
{
    m_rootId = m_index.Create();
    m_index.EmplaceComponent<BaseComponent>(m_rootId, "RootEntity");
    m_index.EmplaceComponent<HierarchyComponent>(m_rootId);
}

Entity Scene::CreateEntity()
{
    return CreateEntity("AnonymousEntity", m_rootId);
}

Entity Scene::CreateEntity(const std::string& name)
{
    return CreateEntity(name, m_rootId);
}

Entity Scene::CreateEntity(const std::string& name, const Entity& parent)
{
    return CreateEntity(name, parent.m_id);
}

Entity Scene::CreateEntity(const std::string& name, const uint32_t& parent)
{
    uint32_t id = m_index.Create();
    m_index.EmplaceComponent<BaseComponent>(id, name);
    m_index.EmplaceComponent<HierarchyComponent>(id, parent, 0, 0, 0);

    AddChild(parent, id);

    return Entity(id, this);
}

void Scene::AddChild(const uint32_t& entity, const uint32_t& child)
{
    HierarchyComponent& parentHierarchy = m_index.GetComponent<HierarchyComponent>(entity);
    uint32_t* nextSibling = &parentHierarchy.firstChild;
    while (*nextSibling) 
    {
        nextSibling = &m_index.GetComponent<HierarchyComponent>(*nextSibling).nextSibling;
    }
    *nextSibling = child;

    parentHierarchy.childCount += 1;
}

Entity Scene::CopyEntity(const Entity& entity, const std::string& name) 
{
    return CopyEntity(entity, name, GetRootEntity());
}

Entity Scene::CopyEntity(const Entity& source, const std::string& name, const Entity& parent)
{
    if (parent.m_scene != this)
    {
        return Entity();
    }
    
    uint32_t newEntity = CopyEntity(source, parent.m_id, 0);
    m_index.GetComponent<BaseComponent>(newEntity).name = name;
    
    AddChild(parent.m_id, newEntity);

    return Entity(newEntity, this);
}

uint32_t Scene::CopyEntity(const Entity& source, const uint32_t& parent, const uint32_t& nextSibling) 
{
    // Create new Entity and copy the source data into it
    uint32_t newEntity = m_index.Create();
    m_index.SetData(newEntity, source.m_scene->m_index.GetData(source.m_id));

    // Reinitialize hierarchy (since the ids will be different)
    auto& hierarchy = m_index.GetComponent<HierarchyComponent>(newEntity);
    hierarchy = {parent, 0, 0, nextSibling};

    // Appending each children in a reverse order to insert the siblings properly
    auto sourceChildren = source.GetChildren();
    std::reverse(sourceChildren.begin(), sourceChildren.end());
    uint32_t childSibling = 0;
    for (Entity child : sourceChildren)
    {
        childSibling = CopyEntity(child, newEntity, childSibling);
        hierarchy.firstChild = childSibling;
    }
    
    hierarchy.childCount = sourceChildren.size();

    return newEntity;
}

Entity Scene::GetRootEntity()
{
    return Entity(m_rootId, this); 
}

void Scene::RemoveEntity(Entity& entity)
{
    EntityView view(entity);
    std::vector<Entity> descendants;
    std::copy(view.begin(), view.end(), std::back_inserter(descendants));
    std::reverse(descendants.begin(), descendants.end());

    for (auto& descendant : descendants)
    {
        m_index.Remove(entity.m_id);
    }

    entity.m_id = 0;
}

std::string Scene::GetEntityName(const uint32_t& id)
{
    return m_index.GetComponent<BaseComponent>(id).name;
}

uint32_t Scene::GetEntityParent(const uint32_t& id) 
{
    if (id == m_rootId)
        return Entity();

    return m_index.GetComponent<HierarchyComponent>(id).parent;
}

Entity Scene::FindByName(const std::string& name)
{
    for (Entity entity : Traverse() ) 
    {
        if (entity.GetName() == name) 
        {
            return entity;
        }
    }

    return Entity();
}

EntityView Scene::Traverse()
{
    return EntityView(Entity(m_rootId, this));
}

void Scene::SetMainCamera(const Entity& entity)
{
    if (entity.m_scene == this)
        m_mainCamera = entity.m_id;
}

Entity Scene::GetMainCamera()
{
    return Entity(m_mainCamera, this);
}

void Scene::Clear()
{
    m_index.Clear();
}

ScenePtr Scene::Create()
{
    return ScenePtr(new Scene);
}