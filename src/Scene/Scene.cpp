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

    HierarchyComponent& parentHierarchy = m_index.GetComponent<HierarchyComponent>(parent);
    uint32_t* lastChild = &parentHierarchy.firstChild;
    while (*lastChild) 
    {
        lastChild = &m_index.GetComponent<HierarchyComponent>(*lastChild).nextSibling;
    }
    *lastChild = id;

    parentHierarchy.childCount += 1;

    return Entity(id, this);
}

void Scene::RemoveEntity(Entity& entity)
{
    m_index.Remove(entity.m_id);
    entity.m_id = 0;
}

const std::string& Scene::GetEntityName(const uint32_t& id)
{
    return m_index.GetComponent<BaseComponent>(id).name;
}

uint32_t Scene::GetEntityParent(const uint32_t& id) 
{
    if (id == m_rootId)
        return Entity(0, nullptr);

    return m_index.GetComponent<HierarchyComponent>(id).parent;
}

Entity Scene::FindByName(const std::string& name)
{
    // for (auto& [id, nameComp] : m_index.Traverse<NameComponent>() ) {
    //     if (nameComp.name == name) {
    //         return Entity(id, this);
    //     }
    // }
    return Entity();
}

EntityView Scene::Traverse()
{
    return EntityView(Entity(m_rootId, this));
}

void Scene::Clear()
{
    m_index.Clear();
}

ScenePtr Scene::Create()
{
    return ScenePtr(new Scene);
}