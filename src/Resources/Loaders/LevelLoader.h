#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include "Game/Level.h"

#include "Scene/Entity.h"

#include "Renderer/Material.h"

#include "Resources/Resource.h"
#include "Resources/Prefab.h"

#include "Core/Foundations.h"

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

    ResourceHandle<Level> Load(const std::string& path);

    inline ResourceHandle<Level> GetLevel() const { return m_levelHandle; }

private:
    Entity BuildPlayer();
    Entity BuildExit();
    Entity BuildMonster(const std::string& name,
                        const glm::vec2& origin,
                        const std::string& modelIdentifier,
                        const uint32_t& health,
                        const float& strength,
                        const float& speed);
    Entity BuildHeal(const std::string& name,
                     const glm::vec2& origin,
                     const std::string& modelIdentifier,
                     const uint32_t& healing);
    Entity BuildWeapon(const std::string& name,
                       const glm::vec2& origin,
                       const std::string& modelIdentifier,
                       const uint32_t& damage);

    ResourceHandle<Prefab> ProcessAndBuildLevelMap(const ImagePtr& map, const std::string& mapPath);
    Entity BuildDoor(const std::string& name,
                     const glm::vec2& origin,
                     const bool& verticalDoor);
    void BuildMaterials();

    ResourceHandle<Level> m_levelHandle;

    Entity m_player;
    glm::vec2 m_entrancePos{-1.0f};
    glm::vec2 m_exitPos{-1.0f};

    std::vector<std::pair<glm::vec2, bool>> m_doors;

    ResourceHandle<Material> m_floorMat;
    ResourceHandle<Material> m_wallMat;
    ResourceHandle<Material> m_doorMat;
    ResourceHandle<Material> m_waterMat;
};

#endif  // LEVELLOADER_H
