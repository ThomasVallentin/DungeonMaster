#ifndef PREFAB_H
#define PREFAB_H

#include "Scene/Scene.h"

#include "Core/Foundations.h"

class Prefab;
class Entity;

DECLARE_PTR_TYPE(Prefab);


class Prefab
{
public:
    ~Prefab() = default;

    const std::string& GetName() const { return m_name; }
    SceneWeakPtr GetInternalScene() const { return m_scene; }
    Entity GetRootEntity() const;

    static PrefabPtr Create();
    static PrefabPtr Create(const std::string& name);

private:
    explicit Prefab(const std::string& name);

    std::string m_name;
    ScenePtr m_scene;
};

#endif  // PREFAB_H
