#include "LevelLoader.h"

#include "Core/Image.h"
#include "Core/Logging.h"
#include "Core/Resolver.h"

#include "Renderer/Mesh.h"

#include "Scene/Entity.h"
#include "Scene/Components/Game.h"

#include "Resources/Manager.h"

#include "Utils/FileUtils.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <rapidjson/document.h>

namespace LevelCells
{
    static const glm::vec4 Wall     {0.0f, 0.0f, 0.0f, 1.0f};
    static const glm::vec4 Floor    {1.0f, 1.0f, 1.0f, 1.0f};
    static const glm::vec4 Door     {0.667f, 0.467f, 0.133f, 1.0f};
    static const glm::vec4 Water    {0.0f, 0.0f, 1.0f, 1.0f};
    static const glm::vec4 Entrance {1.0f, 0.0f, 0.0f, 1.0f};
    static const glm::vec4 Exit     {0.0f, 1.0f, 0.0f, 1.0f};
}


glm::vec4 GetPixel(const glm::vec4* map, const int& x, const int& y, const int& width, const int& height) {
    if (x < 0 || x >= width || y < 0 || y >= height)
        return glm::vec4(0, 0, 0, 1);

    return map[y * width + x];
}


Entity LevelLoader::BuildPlayer()
{
    // Character controller
    ScenePtr scene = m_scene.Get();

    Entity player = scene->CreateEntity("Player");
    player.EmplaceComponent<Components::Transform>(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f)));
    auto& controller = player.EmplaceComponent<Components::Script>(Components::CreateCharacterController(player));
    
    Entity camera = player.AddChild("Camera");
    auto& cam = camera.EmplaceComponent<Components::Camera>();
    scene->SetMainCamera(camera);
    
    // Weapon
    auto sword = ResourceManager::LoadModel("Models/Sting-Sword.fbx");
    Entity weapon = scene->CopyEntity(sword.Get()->GetRootEntity(), "Weapon", player);
    Components::Transform& weaponTransform = weapon.GetComponent<Components::Transform>();
    weaponTransform.transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.15f, 0.0f, -0.5f)) *
        glm::eulerAngleXYZ((float)M_PI_4, 0.0f, (float)M_PI_4) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(0.01f)) 
    ;

    return player;
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

    // Multiple material declaration
    // Floor
    auto defaultShader = Shader::Open(resolver.Resolve("Shaders/default.vert"),
                                      resolver.Resolve("Shaders/default.frag"));
    auto floorMat = ResourceManager::CreateResource<Material>("floorMaterial", Material::Create(defaultShader), false);
    floorMat.Get()->SetInputTexture("diffuseColor", ResourceManager::LoadTexture("Textures/Cobblestone/Albedo.jpg").Get());

    // Wall
    auto wallMat = ResourceManager::CreateResource<Material>("wallMaterial", Material::Create(defaultShader), false);
    wallMat.Get()->SetInputTexture("diffuseColor", ResourceManager::LoadTexture("Textures/Castle_Wall/Albedo.jpg").Get());

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
            if (pixel == LevelCells::Wall)
            {
                continue;
            }

            // Floor entity
            Entity cell = prefabScene->CreateEntity(std::string("Cell") + std::to_string(x) + std::to_string(y));
            cell.EmplaceComponent<Components::Transform>(glm::translate(glm::mat4(1.0f), glm::vec3(x, 0, -y)));
            cell.EmplaceComponent<Components::Mesh>(mesh);
            cell.EmplaceComponent<Components::RenderMesh>(floorMat);

            // Ceiling entity
            // Entity ceiling = prefabScene->CreateEntity(std::string("Cell") + std::to_string(x) + std::to_string(y));
            // ceiling.EmplaceComponent<Components::Transform>(
            //     glm::rotate(
            //         glm::translate(
            //             glm::mat4(1.0f), 
            //             glm::vec3(0.0f, -1.0f, 0.0f)),
            //         (float)M_PI, 
            //         glm::vec3(1.0f, 0.0f, 0.0f)
            // ));            
            // ceiling.EmplaceComponent<Components::Mesh>(mesh);
            // ceiling.EmplaceComponent<Components::RenderMesh>(floorMat);

            // Wall entity(ies)
            if (GetPixel(pixels, x-1, y, width, height) == LevelCells::Wall)
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
                wall.EmplaceComponent<Components::RenderMesh>(wallMat);
            }
            
            if (GetPixel(pixels, x+1, y, width, height) == LevelCells::Wall)
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
                wall.EmplaceComponent<Components::RenderMesh>(wallMat);
            }

            if (GetPixel(pixels, x, y+1, width, height) == LevelCells::Wall)
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
                wall.EmplaceComponent<Components::RenderMesh>(wallMat);
            }

            if (GetPixel(pixels, x, y-1, width, height) == LevelCells::Wall)
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
                wall.EmplaceComponent<Components::RenderMesh>(wallMat);
            }

        }
    }
    
    return prefab;
}


Entity PushGameEntity(const std::string& name, const std::string& modelPath, const glm::vec3& origin, const ScenePtr& scene)
{
    auto modelHandle = ResourceManager::LoadModel(modelPath);
    Entity root = scene->CopyEntity(modelHandle.Get()->GetRootEntity(), name);
    root.GetComponent<Components::Transform>().transform =
        glm::translate( glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)) , origin);

    return root;
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
    LOG_INFO("Level name : %s", document["name"].GetString());
    
    assert(document.HasMember("floors"));
    assert(document["floors"].IsArray());
    assert(document["floors"].Capacity() > 0);
    const auto& firstFloor = document["floors"][0];

    assert(firstFloor.HasMember("name"));
    assert(firstFloor["name"].IsString());
    assert(firstFloor.HasMember("map"));
    assert(firstFloor["map"].IsString());
    LOG_INFO("  Floor : %s (%s)", 
             firstFloor["name"].GetString(), 
             firstFloor["map"].GetString());

    assert(firstFloor.HasMember("rewards"));
    assert(firstFloor["rewards"].IsArray());

    m_scene = ResourceManager::CreateResource<Scene>(firstFloor["name"].GetString(), false);
    auto scene = m_scene.Get();

    // Build level map
    std::string mapPath = std::filesystem::path(resolvedPath).replace_filename(firstFloor["map"].GetString());
    LOG_INFO("Map path : %s", mapPath.c_str());
    auto floorPrefab = BuildLevelMap(mapPath);
    scene->CopyEntity(floorPrefab.Get()->GetRootEntity(), "Floor");

    // Building the player, camera, weapon
    BuildPlayer();

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

        LOG_INFO("    Reward : %s %s (%s) [%d, %d]",
                 reward["type"].GetString(),
                 reward["name"].GetString(),
                 reward["model"].GetString(),
                 reward["origin"][0].GetInt(),
                 reward["origin"][1].GetInt());

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
