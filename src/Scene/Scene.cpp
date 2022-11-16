#include "Scene.h"

#include "Entity.h"


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

void Scene::Clear()
{
    m_index.Clear();
}

ScenePtr Scene::Create()
{
    return ScenePtr(new Scene);
}