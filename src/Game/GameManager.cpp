#include "GameManager.h"

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

}
