#ifndef SCENE_H
#define SCENE_H

#include "EntityIndex.h"
#include "EntityView.h"

#include "Core/Foundations.h"


class Scene;
class Entity;

DECLARE_PTR_TYPE(Scene);


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


class Scene
{
public:
    ~Scene() = default;

    Entity CreateEntity();
    Entity CreateEntity(const std::string& name);
    Entity CreateEntity(const std::string& name, const Entity& parent);

    Entity CopyEntity(const Entity& entity);

    const Entity& GetRootEntity() const;
    Entity FindByName(const std::string& name);
    
    void RemoveEntity(Entity& entity);
    void Clear();

    EntityView Traverse();

    static ScenePtr Create();

private:
    Scene();
    Entity CreateEntity(const std::string& name, const uint32_t& parent);
    const std::string& GetEntityName(const uint32_t& id);
    uint32_t GetEntityParent(const uint32_t& id);

    EntityIndex m_index;
    uint32_t m_rootId;

    friend Entity;
    friend EntityView;
};

#endif  // SCENE_H