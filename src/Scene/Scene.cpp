#include "Scene.h"

#include "Entity.h"
#include <iostream>

struct BaseComponent 
{
    BaseComponent() = default;
    BaseComponent(const std::string& name) : name(name) {}
    
    std::string name;
};

struct HierarchyComponent 
{
    HierarchyComponent() = default;
    HierarchyComponent(const uint32_t& parent, 
                       const uint32_t& childCount,
                       const uint32_t& firstChild, 
                       const uint32_t& nextSibling) :
        parent(parent),
        childCount(childCount),
        firstChild(firstChild),
        nextSibling(nextSibling) {}

    uint32_t parent = 0;
    uint32_t childCount = 0;
    uint32_t firstChild = 0;
    uint32_t nextSibling = 0;
};


// EntityView

EntityView::EntityView(const Entity& entity) : 
    m_begin(entity.m_id, entity.m_scene), m_end(0, entity.m_scene)
{
    if (!entity.IsRoot())  // If being Entity is root
    {
        m_end = iterator(entity.GetComponent<HierarchyComponent>().nextSibling, entity.m_scene);
    }
}

EntityView::EntityView(const uint32_t& beginId, const uint32_t& endId, Scene* scene) :
    m_begin(beginId, scene), m_end(0, scene)
{
    Entity entity(beginId, scene);
    
    // If being Entity is not the root, set the end point to its next sibling
    if (!entity.IsRoot())
    {
        m_end = iterator(entity.GetComponent<HierarchyComponent>().nextSibling, scene);
    }
}

Entity EntityView::iterator::operator->() 
{
    return Entity(m_id, m_scene);
}

Entity EntityView::iterator::operator*() const
{
    return Entity(m_id, m_scene);
}

EntityView::iterator& EntityView::iterator::operator++()
{
    Entity entity(m_id, m_scene);
    const auto& hierarchy = entity.GetComponent<HierarchyComponent>();
    if (hierarchy.firstChild)
    {
        m_id = hierarchy.firstChild;
        return *this;
    }
    if (hierarchy.nextSibling)
    {
        m_id = hierarchy.nextSibling;
        return *this;
    }

    while (entity = entity.GetParent())
    {
        const auto& parentHierarchy = entity.GetComponent<HierarchyComponent>();
        if (parentHierarchy.nextSibling)
        {
            m_id = parentHierarchy.nextSibling;
            return *this; 
        }
    }

    m_id = 0;
    return *this;
}

// Scene

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