#include "Scene.h"

#include "Entity.h"

Entity EntityView::iterator::operator->() 
{
    return Entity(m_it->first, m_scene);
}

Entity EntityView::iterator::operator*() const
{
    return Entity(m_it->first, m_scene);
}

Entity Scene::CreateEntity()
{
    return CreateEntity("AnonymousEntity");
}

Entity Scene::CreateEntity(const std::string& name)
{
    uint32_t id = m_index.Create();
    m_index.EmplaceComponent<BaseComponent>(id, name);

    return Entity(id, this);
}

void Scene::RemoveEntity(Entity& entity)
{
    m_index.Remove(entity.m_id);
    entity.m_id = 0;
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
    return EntityView(m_index.GetDataMap().begin(), 
                      m_index.GetDataMap().end(),
                      this);
}

void Scene::Clear()
{
    m_index.Clear();
}

ScenePtr Scene::Create()
{
    return ScenePtr(new Scene);
}