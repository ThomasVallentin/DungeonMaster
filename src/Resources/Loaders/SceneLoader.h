#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "Resources/Resource.h"
#include "Resources/Prefab.h"


class SceneLoader
{
public:
    SceneLoader() = default;
    ~SceneLoader() = default;

    ResourceHandle<Prefab> Load(const std::string& path);

private:
    ResourceHandle<Prefab> m_prefab;
};

#endif  // SCENELOADER_H
