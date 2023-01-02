#include "LevelLoader.h"

#include "Core/Image.h"
#include "Core/Logging.h"
#include "Core/Resolver.h"

#include "Renderer/Mesh.h"

#include "Scene/Entity.h"

#include "Navigation/Components.h"
#include "Game/Components.h"
#include "Scripting/Trigger.h"

#include "Resources/Manager.h"

#include "Utils/FileUtils.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <rapidjson/document.h>


#define ASSERT_LEVEL_DATA(condition, ...)   if (!(condition)) { LOG_ERROR(__VA_ARGS__); return ResourceHandle<Level>(); }
#define ASSERT_AND_FREE_LEVEL_DATA(condition, path, ...)   if (!(condition)) {                                         \
                                                                          ResourceManager::FreeResource<Level>(path);  \
                                                                          LOG_ERROR(__VA_ARGS__);                                 \
                                                                          return ResourceHandle<Level>(); }


glm::vec4 GetPixel(const glm::vec4* map, const int& x, const int& y, const int& width, const int& height) {
    if (x < 0 || x >= width || y < 0 || y >= height)
        return glm::vec4(0, 0, 0, 1);

    return map[y * width + x];
}


Entity LevelLoader::BuildPlayer()
{
    ScenePtr scene = m_levelHandle.Get()->scene;

    // Main Components
    Entity player = scene->CreateEntity("Player");
    player.EmplaceComponent<Components::Transform>(glm::translate(glm::mat4(1.0f), glm::vec3(m_entrancePos.x, 0.5f, m_entrancePos.y)));
    auto& controller = player.EmplaceComponent<Components::Scriptable>(Components::CreateCharacterController(player));
    player.EmplaceComponent<Components::NavAgent>(player);
    player.EmplaceComponent<Components::CharacterData>(10.0f);
    
    // Camera
    Entity camera = player.AddChild("Camera");
    auto& cam = camera.EmplaceComponent<Components::Camera>();
    scene->SetMainCamera(camera);
    
    // Weapon
    auto sword = ResourceManager::LoadModel("Models/Sword.fbx");
    Entity weapon = scene->CopyEntity(sword.Get()->GetRootEntity(), "Weapon", player);
    weapon.GetComponent<Components::Transform>().transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.06f, -0.08f, -0.15f)) *
        glm::eulerAngleXYZ(0.0f, -(float)M_PI * 0.33f, (float)M_PI * 0.1f);
    weapon.EmplaceComponent<Components::WeaponData>(1.0);

    auto arm = ResourceManager::LoadModel("Models/arm.fbx");
    Entity armEntity = scene->CopyEntity(arm.Get()->GetRootEntity(), "Arm", player);
    armEntity.GetComponent<Components::Transform>().transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.05));

    return player;
}

Entity LevelLoader::BuildExit()
{
    if (m_exitPos == glm::vec2(-1.0f))
    {
        LOG_WARNING("Exit position was not found, moving it to (1, 1) !");
        m_exitPos = glm::vec2(1.0f);
    }

    ScenePtr scene = m_levelHandle.Get()->scene;

    // Main Components
    Entity exit = scene->CreateEntity("Exit");
    exit.EmplaceComponent<Components::Transform>(glm::translate(glm::mat4(1.0f), glm::vec3(m_exitPos.x, 0.5f, m_exitPos.y)));
    exit.EmplaceComponent<Components::Trigger>(exit, m_player, 0.2);
    exit.EmplaceComponent<Components::Scriptable>(Components::CreateExitLogic(exit));

    return exit;
}

Entity LevelLoader::BuildMonster(const std::string& name,
                                 const glm::vec2& origin,
                                 const std::string& modelIdentifier,
                                 const uint32_t& health,
                                 const float& strength,
                                 const float& attackSpeed,
                                 const float& speed)
{
    ScenePtr scene = m_levelHandle.Get()->scene;

    // Character controller
    Entity monster = scene->CreateEntity(name);
    monster.EmplaceComponent<Components::Transform>(glm::translate(glm::mat4(1.0f), 
                                                    glm::vec3(origin.x, 0.0f, -origin.y)));
    auto& monsterData = monster.EmplaceComponent<Components::MonsterData>();
    monsterData.target = m_player;
    monsterData.strength = strength;
    monsterData.attackSpeed = attackSpeed;

    auto& navAgent = monster.EmplaceComponent<Components::NavAgent>(monster);
    navAgent.GetAgent()->SetSpeed(speed);

    auto& logic = monster.EmplaceComponent<Components::Scriptable>(Components::CreateMonsterLogic(monster));
    monster.EmplaceComponent<Components::CharacterData>(health);

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


Entity LevelLoader::BuildHeal(const std::string& name,
                    const glm::vec2& origin,
                    const std::string& modelIdentifier,
                    const uint32_t& healing)
{
    ScenePtr scene = m_levelHandle.Get()->scene;

    // Create entity & components
    Entity entity = scene->CreateEntity(name);
    entity.EmplaceComponent<Components::Transform>(glm::translate(glm::mat4(1.0f), 
                                                    glm::vec3(origin.x, 0.5f, -origin.y)));
    entity.EmplaceComponent<Components::Trigger>(entity, m_player, 0.2);
    auto& logic = entity.EmplaceComponent<Components::Scriptable>(Components::CreateHealLogic(entity));
    logic.GetDataBlock<Components::HealData>().healing = healing;

    // Load model
    ResourceHandle<Prefab> model = ResourceManager::LoadModel(modelIdentifier);
    if (model)
    {
        Entity modelEntity = scene->CopyEntity(model.Get()->GetRootEntity(), "model", entity);
        modelEntity.EmplaceComponent<Components::Scriptable>(Components::CreateRewardAnimator(modelEntity));
    }
    else 
    {
        LOG_WARNING("Could not find model of %s", name.c_str());
    }

    return entity;
}

Entity LevelLoader::BuildWeapon(const std::string& name,
                    const glm::vec2& origin,
                    const std::string& modelIdentifier,
                    const uint32_t& damage)
{
    ScenePtr scene = m_levelHandle.Get()->scene;

    // Create entity & components
    Entity entity = scene->CreateEntity(name);
    entity.EmplaceComponent<Components::Transform>(glm::translate(glm::mat4(1.0f), 
                                                   glm::vec3(origin.x, 0.5f, -origin.y)));
    entity.EmplaceComponent<Components::Trigger>(entity, m_player, 0.2);
    entity.EmplaceComponent<Components::Scriptable>(Components::CreateWeaponLogic(entity));
    entity.EmplaceComponent<Components::WeaponData>(damage);

    // Load model
    ResourceHandle<Prefab> model = ResourceManager::LoadModel(modelIdentifier);
    if (model)
    {
        Entity modelEntity = scene->CopyEntity(model.Get()->GetRootEntity(), "model", entity);
        modelEntity.EmplaceComponent<Components::Scriptable>(Components::CreateRewardAnimator(modelEntity));
    }
    else 
    {
        LOG_WARNING("Could not find model of %s", name.c_str());
    }

    return entity;
}

void LevelLoader::BuildMaterials()
{
    Resolver& resolver = Resolver::Get();

    auto defaultShader = Shader::Open(resolver.Resolve("Shaders/default.vert"),
                                      resolver.Resolve("Shaders/default.frag"));

    // Floor
    if(!(m_floorMat = ResourceManager::GetResource<Material>("floorMaterial")))
    {
        m_floorMat = ResourceManager::CreateResource<Material>("floorMaterial", Material::Create(defaultShader), false);
        m_floorMat.Get()->SetInputTexture("diffuseColor", ResourceManager::LoadTexture("Textures/Cobblestone/Albedo.jpg").Get());
    }

    // Wall
    if(!(m_wallMat = ResourceManager::GetResource<Material>("wallMaterial")))
    {
        m_wallMat = ResourceManager::CreateResource<Material>("wallMaterial", Material::Create(defaultShader), false);
        m_wallMat.Get()->SetInputTexture("diffuseColor", ResourceManager::LoadTexture("Textures/Castle_Wall/Albedo.jpg").Get());
    }

    // Door
    if(!(m_doorMat = ResourceManager::GetResource<Material>("doorMaterial")))
    {
        m_doorMat = ResourceManager::CreateResource<Material>("doorMaterial", Material::Create(defaultShader), false);
        m_doorMat.Get()->SetInputTexture("diffuseColor", ResourceManager::LoadTexture("Textures/Metal_Door/Albedo.jpg").Get());
    }

    // Water
    if(!(m_waterMat = ResourceManager::GetResource<Material>("waterMaterial")))
    {
        m_waterMat = ResourceManager::CreateResource<Material>("waterMaterial",
                                                            Material::Create(Shader::Open(resolver.Resolve("Shaders/default.vert"),
                                                                                          resolver.Resolve("Shaders/water.frag"))), false);
        m_waterMat.Get()->SetInputValue("surfaceColor", glm::vec3(0.0, 0.1, 0.2));
        m_waterMat.Get()->SetInputValue("deepColor", glm::vec3(0.0, 0.25, 0.5));
    }
}


ResourceHandle<Prefab> LevelLoader::ProcessAndBuildLevelMap(const ImagePtr& map, const std::string& mapPath)
{
    Resolver& resolver = Resolver::Get();

    std::string identifier = resolver.AsIdentifier(mapPath);
    ResourceHandle<Prefab> prefab = ResourceManager::GetResource<Prefab>(identifier);
    if (prefab)
    {
        const glm::vec4* pixels = map->GetPixels();
        const uint32_t width = map->GetWidth();
        const uint32_t height = map->GetHeight();

        // The level prefab already exists, just gather the data for the rest of the level building
        for (int y=0 ; y < height ; y++)
        {
            for (size_t x=0 ; x < width ; x++)
            {
                glm::vec4 pixel = GetPixel(pixels, x, y, width, height);
                if (pixel == LevelCell::Door)
                {
                    bool verticalDoor = (GetPixel(pixels, x, y + 1, width, height) == LevelCell::Wall);
                    m_doors.emplace_back(glm::vec2(x, -y), verticalDoor);
                }

                else if (pixel == LevelCell::Entrance)
                {
                    if (m_entrancePos != glm::vec2(-1.0f))
                        LOG_WARNING("Multiple entrances have been specified, using the first one.");
                    else
                        m_entrancePos = glm::vec2(x, -y);
                }

                else if (pixel == LevelCell::Exit)
                {
                    if (m_exitPos != glm::vec2(-1.0f))
                        LOG_WARNING("Multiple exits have been specified, using the first one.");
                    else
                        m_exitPos = glm::vec2(x, -y);
                }
            }
        }

        return prefab;
    }

    prefab = ResourceManager::CreateResource<Prefab>(identifier);
    ScenePtr prefabScene = prefab.Get()->GetInternalScene().lock();
    
    // A single mesh is used since all the level is only composed of quads
    // Keeping each one separated to allow us to use separated materials and culling 
    std::string quadIdentifier = identifier + "QuadMesh";
    ResourceHandle<Mesh> mesh = ResourceManager::GetResource<Mesh>(quadIdentifier);
    if (!mesh)
    {
        std::vector<Vertex> vertices = {{{ 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                        {{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
                                        {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                                        {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}};
        std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};
        mesh = ResourceManager::CreateResource<Mesh>(quadIdentifier,
                                                    Mesh::Create(vertices, indices),
                                                    true);
    }

    const glm::vec4* pixels = map->GetPixels();
    const uint32_t width = map->GetWidth();
    const uint32_t height = map->GetHeight();

    // Processing the image to generate the level
    for (int y=0 ; y < height ; y++)
    {
        for (size_t x=0 ; x < width ; x++)
        {
            glm::vec4 pixel = GetPixel(pixels, x, y, width, height);
            if (pixel == LevelCell::Wall)
                continue;

            // Skip the most common case as quickly as possible to avoid having to proceed too many if statements
            if (pixel != LevelCell::Floor) {
                if (pixel == LevelCell::Door)
                {
                    bool verticalDoor = (GetPixel(pixels, x, y + 1, width, height) == LevelCell::Wall);
                    m_doors.emplace_back(glm::vec2(x, -y), verticalDoor);
                }

                else if (pixel == LevelCell::Entrance)
                {
                    if (m_entrancePos != glm::vec2(-1.0f))
                        LOG_WARNING("Multiple entrances have been specified, using the first one.");
                    else
                        m_entrancePos = glm::vec2(x, -y);
                }

                else if (pixel == LevelCell::Exit)
                {
                    if (m_exitPos != glm::vec2(-1.0f))
                        LOG_WARNING("Multiple exits have been specified, using the first one.");
                    else
                        m_exitPos = glm::vec2(x, -y);
                }
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

Entity LevelLoader::BuildDoor(const std::string& name,
                              const glm::vec2& origin,
                              const bool& verticalDoor) 
{
    ScenePtr scene = m_levelHandle.Get()->scene;

    // Create entity & components
    Entity door = scene->CreateEntity(name);
    glm::mat4 doorMatrix = glm::translate(glm::mat4(1.0f), 
                                          glm::vec3(origin.x, 0.5f, origin.y));
    if (verticalDoor)
    {
        doorMatrix = glm::rotate(doorMatrix, (float)M_PI_2, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    door.EmplaceComponent<Components::Transform>(doorMatrix);
    door.EmplaceComponent<Components::Trigger>(door, m_player, 2.0f);
    door.EmplaceComponent<Components::Scriptable>(Components::CreateDoorLogic(door));

    // A door is a simple quad for now, should be replaced by a proper model
    std::string identifier = "DoorMesh";
    ResourceHandle<Mesh> mesh = ResourceManager::GetResource<Mesh>(identifier);
    if (!mesh)
    {
        std::vector<Vertex> vertices = {{{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
                                        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                                        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                                        {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}};
        std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};
        mesh = ResourceManager::CreateResource<Mesh>(identifier,
                                                    Mesh::Create(vertices, indices),
                                                    true);
    }

    Entity model = door.AddChild("model");
    model.EmplaceComponent<Components::Transform>();
    model.EmplaceComponent<Components::Mesh>(mesh);
    model.EmplaceComponent<Components::RenderMesh>(m_doorMat, true);

    return door;
}

ResourceHandle<Level> LevelLoader::Load(const std::string& path)
{
    Resolver& resolver = Resolver::Get();
    std::string resolvedPath = resolver.Resolve(path);

    std::string content;
    if (!ReadFile(resolvedPath, content))
    {
        LOG_ERROR("Could not load level %s", path.c_str());
        return m_levelHandle;
    }

    rapidjson::Document document;
    document.Parse(content.c_str());

    ASSERT_LEVEL_DATA(document.HasMember("name"), "Invalid level name.");
    ASSERT_LEVEL_DATA(document["name"].IsString(), "Invalid level name.");
    LOG_DEBUG("LevelLoader : Loading level %s (%s)", document["name"].GetString(), path);
    
    ASSERT_LEVEL_DATA(document.HasMember("floors"), "Invalid floors data.");
    ASSERT_LEVEL_DATA(document["floors"].IsArray(), "Invalid floors data.");
    ASSERT_LEVEL_DATA(document["floors"].Capacity() > 0, "Invalid floors data.");
    const auto& firstFloor = document["floors"][0];

    ASSERT_LEVEL_DATA(firstFloor.HasMember("name"), "Invalid floor data.");
    ASSERT_LEVEL_DATA(firstFloor["name"].IsString(), "Invalid floor data.");
    ASSERT_LEVEL_DATA(firstFloor.HasMember("map"), "Invalid floor data.");
    ASSERT_LEVEL_DATA(firstFloor["map"].IsString(), "Invalid floor data.");

    // Initialize the resource
    m_levelHandle = ResourceManager::CreateResource<Level>(path, false);
    auto level = m_levelHandle.Get();
    level->scene = Scene::Create();
    level->floorName = firstFloor["name"].GetString();

    // Reading the map
    std::string mapPath = std::filesystem::path(resolvedPath).replace_filename(firstFloor["map"].GetString());
    level->map = Image::Read(mapPath);
    ASSERT_LEVEL_DATA(level->map, "Could not open the level map");

    // Build level map
    BuildMaterials();
    auto floorPrefab = ProcessAndBuildLevelMap(level->map, mapPath);
    level->scene->CopyEntity(floorPrefab.Get()->GetRootEntity(), "Floor");

    // Build the player
    m_player = BuildPlayer();

    // Build doors
    for (size_t i = 0 ; i < m_doors.size() ; i++)
    {
        BuildDoor(std::string("Door") + std::to_string(i),
                  m_doors[i].first,
                  m_doors[i].second);
    }

    // Build the monsters
    if (firstFloor.HasMember("monsters") && firstFloor["monsters"].IsArray())
    {
        for (const auto& monster : firstFloor["monsters"].GetArray())
        {
            ASSERT_AND_FREE_LEVEL_DATA(monster.HasMember("name"), path, "Invalid monster name.");
            ASSERT_AND_FREE_LEVEL_DATA(monster["name"].IsString(), path, "Invalid monster name.");
            
            ASSERT_AND_FREE_LEVEL_DATA(monster.HasMember("origin"), path, "Invalid monster origin.");
            ASSERT_AND_FREE_LEVEL_DATA(monster["origin"].IsArray(), path, "Invalid monster origin.");
            ASSERT_AND_FREE_LEVEL_DATA(monster["origin"].Capacity() == 2, path, "Invalid monster origin.");
            
            ASSERT_AND_FREE_LEVEL_DATA(monster.HasMember("model"), path, "Invalid monster model.");
            ASSERT_AND_FREE_LEVEL_DATA(monster["model"].IsString(), path, "Invalid monster model.");
            
            ASSERT_AND_FREE_LEVEL_DATA(monster.HasMember("health"), path, "Invalid monster health.");
            ASSERT_AND_FREE_LEVEL_DATA(monster["health"].IsInt(), path, "Invalid monster health.");

            ASSERT_AND_FREE_LEVEL_DATA(monster.HasMember("strength"), path, "Invalid monster strength.");
            ASSERT_AND_FREE_LEVEL_DATA(monster["strength"].IsFloat(), path, "Invalid monster strength.");

            ASSERT_AND_FREE_LEVEL_DATA(monster.HasMember("attackSpeed"), path, "Invalid monster attack speed.");
            ASSERT_AND_FREE_LEVEL_DATA(monster["attackSpeed"].IsFloat(), path, "Invalid monster attack speed.");

            ASSERT_AND_FREE_LEVEL_DATA(monster.HasMember("speed"), path, "Invalid monster speed.");
            ASSERT_AND_FREE_LEVEL_DATA(monster["speed"].IsFloat(), path, "Invalid monster speed.");

            auto origin = monster["origin"].GetArray();
            BuildMonster(monster["name"].GetString(),
                        glm::vec2(origin[0].GetFloat(), origin[1].GetFloat()),
                        monster["model"].GetString(),
                        monster["health"].GetInt(),
                        monster["strength"].GetFloat(),
                        monster["attackSpeed"].GetFloat(),
                        monster["speed"].GetFloat());
        }
    }

    if(firstFloor.HasMember("rewards") && firstFloor["rewards"].IsArray())
    {
        for (const auto& reward : firstFloor["rewards"].GetArray())
        {
            ASSERT_AND_FREE_LEVEL_DATA(reward.HasMember("name"), path, "Invalid reward name.");
            ASSERT_AND_FREE_LEVEL_DATA(reward["name"].IsString(), path, "Invalid reward name.");
            
            ASSERT_AND_FREE_LEVEL_DATA(reward.HasMember("origin"), path, "Invalid reward origin.");
            ASSERT_AND_FREE_LEVEL_DATA(reward["origin"].IsArray(), path, "Invalid reward origin.");
            ASSERT_AND_FREE_LEVEL_DATA(reward["origin"].Capacity() == 2, path, "Invalid reward origin.");
            
            ASSERT_AND_FREE_LEVEL_DATA(reward.HasMember("model"), path, "Invalid reward model.");
            ASSERT_AND_FREE_LEVEL_DATA(reward["model"].IsString(), path, "Invalid reward model.");
            
            ASSERT_AND_FREE_LEVEL_DATA(reward.HasMember("type"), path, "Invalid reward type.");
            ASSERT_AND_FREE_LEVEL_DATA(reward["type"].IsString(), path, "Invalid reward type.");

            std::string type = reward["type"].GetString();
            if (type == "weapon")
            {
                ASSERT_AND_FREE_LEVEL_DATA(reward.HasMember("damage"), path, "Invalid weapon damage.");
                ASSERT_AND_FREE_LEVEL_DATA(reward["damage"].IsUint(), path, "Invalid weapon damage.");

                auto origin = reward["origin"].GetArray();
                BuildWeapon(reward["name"].GetString(),
                            glm::vec2(origin[0].GetFloat(), origin[1].GetFloat()),
                            reward["model"].GetString(),
                            reward["damage"].GetUint());
            }

            else if (type == "heal")
            {
                ASSERT_AND_FREE_LEVEL_DATA(reward.HasMember("healing"), path, "Invalid heal healing.");
                ASSERT_AND_FREE_LEVEL_DATA(reward["healing"].IsUint(), path, "Invalid heal healing.");

                auto origin = reward["origin"].GetArray();
                BuildHeal(reward["name"].GetString(),
                        glm::vec2(origin[0].GetFloat(), origin[1].GetFloat()),
                        reward["model"].GetString(),
                        reward["healing"].GetUint());
            }
        }
    }

    BuildExit();

    return m_levelHandle;
}
