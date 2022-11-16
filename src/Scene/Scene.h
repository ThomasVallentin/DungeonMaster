#ifndef SCENE_H
#define SCENE_H

#include "EntityIndex.h"

#include "Core/Foundations.h"


class Scene;
class Entity;

DECLARE_PTR_TYPE(Scene);


struct BaseComponent {
    BaseComponent() = default;
    BaseComponent(const std::string& name) : name(name) {}

    std::string name;
};


class Scene
{
public:
    ~Scene() = default;

    Entity CreateEntity();
    Entity CreateEntity(const std::string& name);
    void RemoveEntity(Entity& entity);

    Entity FindByName(const std::string& name);
    void Clear();

    static ScenePtr Create();

private:
    Scene() = default;

    EntityIndex m_index;

    friend Entity;
};

#endif  // SCENE_H