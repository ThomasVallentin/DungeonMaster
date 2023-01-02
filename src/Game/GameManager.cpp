#include "GameManager.h"
#include "Level.h"

#include "Navigation/Engine.h"

#include "Core/Resolver.h"


GameManager* GameManager::s_instance = nullptr;


GameManager& GameManager::Init()
{
    if (!s_instance)
    {
        s_instance = new GameManager;
    }

    return *s_instance;
}

void GameManager::SetPlayer(const Entity& player)
{
    m_player = player;
}

void GameManager::AddMonster(const Entity& entity)
{
    if (entity.GetScene() != Application::Get().GetMainScene().get())
    {
        return;
    }

    auto it = std::find(m_monsters.begin(), m_monsters.end(), entity);
    if (it == m_monsters.end())
    {
        m_monsters.push_back(entity);
    }
}

void GameManager::RemoveMonster(const Entity& entity)
{
    if (entity.GetScene() != Application::Get().GetMainScene().get())
    {
        return;
    }
    
    auto it = std::find(m_monsters.begin(), m_monsters.end(), entity);
    if (it != m_monsters.end())
    {
        m_monsters.erase(it);
    }
}

void GameManager::Clear()
{
    m_monsters.clear();
    m_player = Entity();
}

void GameManager::SetNextLevel(const std::string& levelIdentifier)
{
    m_nextLevel = levelIdentifier;
}

void GameManager::SetNextFloor(const uint32_t& floor)
{
    m_nextFloor = floor;
}

void GameManager::ShowLooseScreen() const
{
    LOG_INFO("LOOSE !");
    ScenePtr scene = Scene::Create();

    Entity camera = scene->CreateEntity();
    camera.EmplaceComponent<Components::Camera>();

    Entity gameOverScreen = scene->CreateEntity();
    gameOverScreen.EmplaceComponent<Components::RenderImage>(ResourceManager::LoadTexture("Textures/GameOver.png"));
    gameOverScreen.EmplaceComponent<Components::Scriptable>(Components::CreateGameOverLogic(gameOverScreen));

    Application& application = Application::Get();
    application.SetMainScene(scene);
}

void GameManager::StartGame()
{
    if (m_nextLevel.empty())
    {
        m_nextLevel = m_currentLevel;
    }
    
    if (m_nextFloor == -1)
    {
        m_nextFloor = m_currentFloor;
    }

    LoadLevel(m_nextLevel, m_nextFloor);
}

void GameManager::RestartGame()
{
    LoadLevel(m_currentLevel, m_currentFloor);
}


void GameManager::LoadLevel(const std::string& levelIdentifier, const uint32_t& floor)
{
    ResourceManager::FreeResource<Level>(m_currentLevel);

    m_currentLevel = levelIdentifier;
    m_currentFloor = floor;

    auto& resolver = Resolver::Get(); 
    LevelPtr level = ResourceManager::LoadLevel(resolver.Resolve(levelIdentifier)).Get();
    if (!level)
    {
        return;
    }

    Navigation::Engine& navEngine = Navigation::Engine::Get();
    navEngine.SetNavMap(level->map);

    Application& application = Application::Get();
    application.SetMainScene(level->scene);

    // Cleanup
    m_nextLevel.clear();
    m_nextFloor = -1;

    LOG_INFO("Starting %s !", level->floorName.c_str());
}
