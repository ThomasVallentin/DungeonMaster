#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include "Resources/Resource.h"
#include "Resources/Prefab.h"
#include "Renderer/Material.h"

#include <glm/glm.hpp>

class LevelLoader
{
public:
    LevelLoader() = default;
    ~LevelLoader() = default;

    ResourceHandle<Scene> Load(const std::string& path);

private:
    Entity BuildPlayer();
    void BuildMaterials();
    ResourceHandle<Prefab> BuildLevelMap(const std::string& path);

    ResourceHandle<Scene> m_scene;
    ResourceHandle<Material> m_floorMat;
    ResourceHandle<Material> m_wallMat;
    ResourceHandle<Material> m_waterMat;
    glm::vec2 m_playerPos{-1.0f};
};

#endif  // LEVELLOADER_H
