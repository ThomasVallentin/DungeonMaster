#include "LevelLoader.h"

#include "Core/Image.h"
#include "Core/Logging.h"
#include "Core/Resolver.h"

#include "Renderer/Mesh.h"

#include "Scene/Entity.h"

#include "Navigation/Components.h"
#include "Game/Components.h"

#include "Resources/Manager.h"

#include "Utils/FileUtils.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <rapidjson/document.h>


glm::vec4 GetPixel(const glm::vec4* map, const int& x, const int& y, const int& width, const int& height) {
    if (x < 0 || x >= width || y < 0 || y >= height)
        return glm::vec4(0, 0, 0, 1);

    return map[y * width + x];
}

Entity PushGameEntity(const std::string& name, const std::string& modelPath, const glm::vec3& origin, const ScenePtr& scene)
{
    auto modelHandle = ResourceManager::LoadModel(modelPath);
    Entity root = scene->CopyEntity(modelHandle.Get()->GetRootEntity(), name);
    root.GetComponent<Components::Transform>().transform =
        glm::translate( glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)) , origin);

    return root;
}


Entity LevelLoader::BuildPlayer()
{
    if (m_playerPos == glm::vec2(-1.0f))
    {
        LOG_WARNING("Player position was not found, moving it to (0, 0) !");
        m_playerPos = glm::vec2(0.0f);
    }

    ScenePtr scene = m_scene.Get();

    // Character controller
    Entity player = scene->CreateEntity("Player");
    player.EmplaceComponent<Components::Transform>(glm::translate(glm::mat4(1.0f), glm::vec3(m_playerPos.x, 0.5f, m_playerPos.y)));
    auto& controller = player.EmplaceComponent<Components::Scriptable>(Components::CreateCharacterController(player));
    // auto& agent = player.EmplaceComponent<Components::NavAgent>(player);
    
    // Camera
    Entity camera = player.AddChild("Camera");
    auto& cam = camera.EmplaceComponent<Components::Camera>();
    scene->SetMainCamera(camera);
    
    // Weapon
    auto sword = ResourceManager::LoadModel("Models/Sword.fbx");
    Entity weapon = scene->CopyEntity(sword.Get()->GetRootEntity(), "Weapon", player);
    weapon.GetComponent<Components::Transform>().transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.06f, -0.08f, -0.15f)) *
        glm::eulerAngleXYZ(0.0f, -(float)M_PI * 0.33f, (float)M_PI * 0.1f) 
    ;

    auto arm = ResourceManager::LoadModel("Models/arm.fbx");
    Entity armEntity = scene->CopyEntity(arm.Get()->GetRootEntity(), "Arm", player);
    armEntity.GetComponent<Components::Transform>().transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.05));

    return player;
}

Entity LevelLoader::BuildMonster(const std::string& name,
                                 const glm::vec2& origin,
                                 const std::string& modelIdentifier,
                                 const uint32_t& health,
                                 const float& strength,
                                 const float& speed,
                                 const Entity& target)
{
    ScenePtr scene = m_scene.Get();

    // Character controller
    Entity monster = scene->CreateEntity(name);
    monster.EmplaceComponent<Components::Transform>(glm::translate(glm::mat4(1.0f), 
                                                    glm::vec3(origin.x, 0.0f, -origin.y)));
    monster.EmplaceComponent<Components::NavAgent>(monster);
    auto& logic = monster.EmplaceComponent<Components::Scriptable>(Components::CreateMonsterLogic(monster));
    logic.GetDataBlock<Components::MonsterLogicData>().target = m_player;

    ResourceHandle<Prefab> model = ResourceManager::LoadModel(modelIdentifier);
    if (model)
    {
        Entity modelEntity = scene->CopyEntity(model.Get()->GetRootEntity(), "model", monster);
        modelEntity.GetComponent<Components::Transform>().transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.03f)); // TODO: Fix model instead of scaling here
    }
    else 
    {
        LOG_WARNING("Could not find model of %s", name.c_str());
    }

    return monster;
}


void LevelLoader::BuildMaterials()
{
    Resolver& resolver = Resolver::Get();

    auto defaultShader = Shader::Open(resolver.Resolve("Shaders/default.vert"),
                                      resolver.Resolve("Shaders/default.frag"));
    
    // Floor
    m_floorMat = ResourceManager::CreateResource<Material>("floorMaterial", Material::Create(defaultShader), false);
    m_floorMat.Get()->SetInputTexture("diffuseColor", ResourceManager::LoadTexture("Textures/Cobblestone/Albedo.jpg").Get());

    // Wall
    m_wallMat = ResourceManager::CreateResource<Material>("wallMaterial", Material::Create(defaultShader), false);
    m_wallMat.Get()->SetInputTexture("diffuseColor", ResourceManager::LoadTexture("Textures/Castle_Wall/Albedo.jpg").Get());

    // Water
    m_waterMat = ResourceManager::CreateResource<Material>("waterMaterial",
                                                           Material::Create(Shader::Open(resolver.Resolve("Shaders/default.vert"),
                                                                                         resolver.Resolve("Shaders/water.frag"))), false);
    m_waterMat.Get()->SetInputValue("surfaceColor", glm::vec3(0.0, 0.1, 0.2));
    m_waterMat.Get()->SetInputValue("deepColor", glm::vec3(0.0, 0.25, 0.5));
}


ResourceHandle<Prefab> LevelLoader::BuildLevelMap(const std::string& path)
{
    Resolver& resolver = Resolver::Get();

    std::string identifier = resolver.AsIdentifier(path);
    auto prefab = ResourceManager::CreateResource<Prefab>(identifier);
    auto prefabScene = prefab.Get()->GetInternalScene().lock();
    
    // A single mesh is used since all the level is only composed of quads
    // Keeping each one separated to allow us to use separated materials and culling 
    std::vector<Vertex> vertices = {{{ 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                    {{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
                                    {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                                    {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}};
    std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};
    auto mesh = ResourceManager::CreateResource<Mesh>(identifier + "QuadMesh", 
                                                      Mesh::Create(vertices, indices),
                                                      true);

    ImagePtr image = Image::Read(resolver.Resolve(path));
    const glm::vec4* pixels = image->GetPixels();
    const uint32_t width = image->GetWidth();
    const uint32_t height = image->GetHeight();

    // Processing the image to generate the level
    for (int y=0 ; y < height ; y++)
    {
        for (size_t x=0 ; x < width ; x++)
        {
            glm::vec4 pixel = GetPixel(pixels, x, y, width, height);
            if (pixel == LevelCell::Wall)
                continue;

            if (pixel == LevelCell::Entrance)
            {
                if (m_playerPos != glm::vec2(-1.0f))
                    LOG_WARNING("Multiple entrances have been specified, using the first one.");
                else
                    m_playerPos = glm::vec2(x, -y);
            }

            // Floor entity
            Entity cell = prefabScene->CreateEntity(std::string("Cell") + std::to_string(x) + std::to_string(y));
            cell.EmplaceComponent<Components::Transform>(glm::translate(glm::mat4(1.0f), glm::vec3(x, 0, -y)));
            cell.EmplaceComponent<Components::Mesh>(mesh);
            if (pixel == LevelCell::Water)
                cell.EmplaceComponent<Components::RenderMesh>(m_waterMat);
            else
                cell.EmplaceComponent<Components::RenderMesh>(m_floorMat);

            // Ceiling entity
            Entity ceiling = prefabScene->CreateEntity(std::string("Ceiling") + std::to_string(x) + std::to_string(y), cell);
            ceiling.EmplaceComponent<Components::Transform>(
                glm::rotate(
                    glm::translate(
                        glm::mat4(1.0f), 
                        glm::vec3(0.0f, 1.0f, 0.0f)),
                    (float)M_PI, 
                    glm::vec3(1.0f, 0.0f, 0.0f)
            ));            
            ceiling.EmplaceComponent<Components::Mesh>(mesh);
            ceiling.EmplaceComponent<Components::RenderMesh>(m_floorMat);

            // Wall entity(ies)
            if (GetPixel(pixels, x-1, y, width, height) == LevelCell::Wall)
            {
                Entity wall = prefabScene->CreateEntity("leftWall", cell);
                wall.EmplaceComponent<Components::Transform>(
                    glm::rotate(
                        glm::translate(
                            glm::rotate(
                                glm::mat4(1.0f), 
                                (float)M_PI_2, 
                                glm::vec3(0, 1, 0)),
                            glm::vec3(0.0, 0.5, -0.5)),
                        (float)M_PI_2, 
                        glm::vec3(1, 0, 0)
                ));
                wall.EmplaceComponent<Components::Mesh>(mesh);
                wall.EmplaceComponent<Components::RenderMesh>(m_wallMat);
            }
            
            if (GetPixel(pixels, x+1, y, width, height) == LevelCell::Wall)
            {
                Entity wall = prefabScene->CreateEntity("rightWall", cell);
                wall.EmplaceComponent<Components::Transform>(
                    glm::rotate(
                        glm::translate(
                            glm::rotate(
                                glm::mat4(1.0f), 
                                -(float)M_PI_2, 
                                glm::vec3(0, 1, 0)),
                            glm::vec3(0.0, 0.5, -0.5)),
                        (float)M_PI_2, 
                        glm::vec3(1, 0, 0)
                ));
                wall.EmplaceComponent<Components::Mesh>(mesh);
                wall.EmplaceComponent<Components::RenderMesh>(m_wallMat);
            }

            if (GetPixel(pixels, x, y+1, width, height) == LevelCell::Wall)
            {
                Entity wall = prefabScene->CreateEntity("topWall", cell);
                wall.EmplaceComponent<Components::Transform>(
                    glm::rotate(
                        glm::translate(
                            glm::mat4(1.0f), 
                            glm::vec3(0.0, 0.5, -0.5)),
                        (float)M_PI_2, 
                        glm::vec3(1, 0, 0)
                ));
                wall.EmplaceComponent<Components::Mesh>(mesh);
                wall.EmplaceComponent<Components::RenderMesh>(m_wallMat);
            }

            if (GetPixel(pixels, x, y-1, width, height) == LevelCell::Wall)
            {
                Entity wall = prefabScene->CreateEntity("bottomWall", cell);
                wall.EmplaceComponent<Components::Transform>(

                                    glm::rotate(
                        glm::translate(
                            glm::rotate(
                                glm::mat4(1.0f), 
                                (float)M_PI, 
                                glm::vec3(0, 1, 0)),
                            glm::vec3(0.0, 0.5, -0.5)),
                        (float)M_PI_2, 
                        glm::vec3(1, 0, 0)
                ));
                wall.EmplaceComponent<Components::Mesh>(mesh);
                wall.EmplaceComponent<Components::RenderMesh>(m_wallMat);
            }
        }
    }
    
    return prefab;
}

ResourceHandle<Scene> LevelLoader::Load(const std::string& path)
{
    Resolver& resolver = Resolver::Get();
    std::string resolvedPath = resolver.Resolve(path);

    std::string content;
    if (!ReadFile(resolvedPath, content))
    {
        LOG_ERROR("Could not load level %s", path.c_str());
    }

    rapidjson::Document document;
    document.Parse(content.c_str());

    assert(document.HasMember("name"));
    assert(document["name"].IsString());
    LOG_DEBUG("LevelLoader : Loading level %s (%s)", document["name"].GetString(), path);
    
    assert(document.HasMember("floors"));
    assert(document["floors"].IsArray());
    assert(document["floors"].Capacity() > 0);
    const auto& firstFloor = document["floors"][0];

    assert(firstFloor.HasMember("name"));
    assert(firstFloor["name"].IsString());
    assert(firstFloor.HasMember("map"));
    assert(firstFloor["map"].IsString());

    m_scene = ResourceManager::CreateResource<Scene>(firstFloor["name"].GetString(), false);
    auto scene = m_scene.Get();

    BuildMaterials();

    // Build level map
    std::string mapPath = std::filesystem::path(resolvedPath).replace_filename(firstFloor["map"].GetString());
    auto floorPrefab = BuildLevelMap(mapPath);
    scene->CopyEntity(floorPrefab.Get()->GetRootEntity(), "Floor");

    // Building the player, camera, weapon
    m_player = BuildPlayer();

    assert(firstFloor.HasMember("monsters"));
    assert(firstFloor["monsters"].IsArray());

    for (const auto& monster : firstFloor["monsters"].GetArray())
    {
        assert(monster.HasMember("name"));
        assert(monster["name"].IsString());
        
        assert(monster.HasMember("origin"));
        assert(monster["origin"].IsArray());
        assert(monster["origin"].Capacity() == 2);
        
        assert(monster.HasMember("model"));
        assert(monster["model"].IsString());
        
        assert(monster.HasMember("health"));
        assert(monster["health"].IsInt());

        assert(monster.HasMember("strength"));
        assert(monster["strength"].IsFloat());

        assert(monster.HasMember("speed"));
        assert(monster["speed"].IsFloat());

        auto origin = monster["origin"].GetArray();
        BuildMonster(monster["name"].GetString(),
                     glm::vec2(origin[0].GetFloat(), origin[1].GetFloat()),
                     monster["model"].GetString(),
                     monster["health"].GetInt(),
                     monster["strength"].GetFloat(),
                     monster["speed"].GetFloat(),
                     m_player);
    }


    assert(firstFloor.HasMember("rewards"));
    assert(firstFloor["rewards"].IsArray());

    for (const auto& reward : firstFloor["rewards"].GetArray())
    {
        assert(reward.HasMember("name"));
        assert(reward["name"].IsString());
        
        assert(reward.HasMember("origin"));
        assert(reward["origin"].IsArray());
        assert(reward["origin"].Capacity() == 2);
        
        assert(reward.HasMember("model"));
        assert(reward["model"].IsString());
        
        assert(reward.HasMember("type"));
        assert(reward["type"].IsString());

        Entity rewardEntity = PushGameEntity(reward["name"].GetString(), 
                                             reward["model"].GetString(), 
                                             glm::vec3(reward["origin"][0].GetInt(), 0.0, reward["origin"][1].GetInt()),
                                             scene);

        std::string type = reward["type"].GetString();
        if (type == "weapon")
        {
            assert(reward.HasMember("damage"));
            assert(reward["damage"].IsUint());


        }

        else if (type == "heal")
        {
            assert(reward.HasMember("healing"));
            assert(reward["healing"].IsUint());

        }
    }

    return m_scene;
}
