#include "Prefab.h"

Prefab::Prefab(const std::string& name) :
        m_name(name), 
        m_scene(Scene::Create())
{
}

PrefabPtr Prefab::Create()
{
    return PrefabPtr(new Prefab("UnnamedPrefab"));
}

PrefabPtr Prefab::Create(const std::string& name)
{
    return PrefabPtr(new Prefab(name));
}
