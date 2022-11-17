#include "EntityView.h"

#include "Entity.h"
#include "Scene.h"


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