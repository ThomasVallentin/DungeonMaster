#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include "Resources/Resource.h"
#include "Resources/Prefab.h"
#include "Renderer/Material.h"

#include <glm/glm.hpp>

namespace LevelCell
{
    static const glm::vec4 Wall     {0.0f, 0.0f, 0.0f, 1.0f};
    static const glm::vec4 Floor    {1.0f, 1.0f, 1.0f, 1.0f};
    static const glm::vec4 Door     {170.0f/255.0f, 119.0f/255.0f, 34.0f/255.0f, 1.0f};
    static const glm::vec4 Water    {0.0f, 0.0f, 1.0f, 1.0f};
    static const glm::vec4 Entrance {1.0f, 0.0f, 0.0f, 1.0f};
    static const glm::vec4 Exit     {0.0f, 1.0f, 0.0f, 1.0f};
}

class LevelLoader
{
public:
    LevelLoader() = default;
    ~LevelLoader() = default;

    ResourceHandle<Scene> Load(const std::string& path);

private:
    Entity BuildPlayer();
    Entity BuildMonster(const std::string& name,
                        const glm::vec2& origin,
                        const std::string& modelIdentifier,
                        const uint32_t& health,
                        const float& strength,
                        const float& speed);
    void BuildMaterials();
    ResourceHandle<Prefab> BuildLevelMap(const std::string& path);

    ResourceHandle<Scene> m_scene;
    ResourceHandle<Material> m_floorMat;
    ResourceHandle<Material> m_wallMat;
    ResourceHandle<Material> m_waterMat;
    glm::vec2 m_playerPos{-1.0f};
};

#endif  // LEVELLOADER_H
