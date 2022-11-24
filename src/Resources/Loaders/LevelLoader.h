#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include "Resources/Resource.h"
#include "Resources/Prefab.h"


class LevelLoader
{
public:
    LevelLoader() = default;
    ~LevelLoader() = default;

    ResourceHandle<Scene> Load(const std::string& path);

private:
    Entity BuildPlayer();
    ResourceHandle<Prefab> BuildLevelMap(const std::string& path);

    ResourceHandle<Scene> m_scene;
};

#endif  // LEVELLOADER_H
