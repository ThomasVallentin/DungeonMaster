#include "Application.h"

#include "Game/GameManager.h"

#include "Scene/Entity.h"
#include "Scene/Components/Basics.h"

#include "Navigation/Engine.h"
#include "Navigation/Agent.h"
#include "Navigation/Components.h"

#include "Scripting/Trigger.h"

#include "Renderer/Renderer.h"

#include "Resources/Model.h"
#include "Resources/Manager.h"

#include "Resolver.h"
#include "Window.h"
#include "Event.h"
#include "Time.h"
#include "Logging.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glad/glad.h>

#include <sstream>


Application* Application::s_instance = nullptr;

double Time::s_time = 0.0f;
double Time::s_deltaTime = 0.0f;

Application& Application::Init(int argc, char* argv[])
{
    if (Application::s_instance) {
        LOG_WARNING("Application already exists, cannot Init() it twice.");
        return *s_instance;
    }

    s_instance = new Application(argc, argv);

    return *s_instance;
}


Application::Application(int argc, char* argv[])
{
    std::string appPath = argv[0];
    Resolver& resolver = Resolver::Init(std::filesystem::canonical(appPath).remove_filename().parent_path().parent_path());

    Scripting::Engine::Init();
    Navigation::Engine::Init();
    
    GameManager& gameManager = GameManager::Init();
    gameManager.SetNextLevel("Levels/Labyrinth.json");

    m_window = std::make_unique<Window>(WindowSettings{1280, 720, "Dungeon Master"});

    Renderer& renderer = Renderer::Init();
    renderer.SetClearColor(glm::vec3(0.0f, 0.0f, 0.0f));
    renderer.SetRenderBuffer(FrameBuffer::Create({ 1280, 720, 8, {GL_RGBA32F} }));
    renderer.SetPostProcessShader(Shader::Open(resolver.Resolve("Shaders/fullScreen.vert"), 
                                               resolver.Resolve("Shaders/postProcess.frag")));

    m_scene = Scene::Create();
}

void Application::Run()
{
    m_isRunning = true;

    GameManager::Get().ShowTitleScreen();

    while (m_isRunning)
    {
        Time::SetTime(m_window->GetInternalTime());

        OnUpdate();
        m_window->OnUpdate();
        
        std::ostringstream titleStream;
        titleStream << std::fixed << std::setprecision(2);
        titleStream << "Dungeon Master | " << (Time::GetDeltaTime()) * 1000 << "ms/frame";
        m_window->SetTitle(titleStream.str());
    }
}

void Application::OnUpdate() 
{
    double time = Time::GetTime();

    // Switching scenes in a safe spot to avoid unwanted accesses
    if (m_nextScene)
    {
        SwitchScenes();
    }

    // Update Navigation
    Navigation::Engine& navEngine = Navigation::Engine::Get();
    navEngine.OnUpdate();

    // Updating Scripts
    Scripting::Engine::Get().OnUpdate();

    Renderer& renderer = Renderer::Get();

    renderer.ClearBuffer(0);
    renderer.RenderScene(m_scene, m_scene->GetMainCamera());
    renderer.BlitRenderToBuffer(0);
}

void Application::Stop()
{
    m_isRunning = false; 
}

void Application::EmitEvent(Event* event)
{
    switch (event->GetType()) 
    {
        case EventType::WindowClosed:
        {
            m_isRunning = false;
            break;
        }

        case EventType::WindowResized:
        {
            WindowResizedEvent* resizeEvent = dynamic_cast<WindowResizedEvent*>(event);

            auto* mainCamera = m_scene->GetMainCamera().FindComponent<Components::Camera>();
            if (mainCamera)
            {
                mainCamera->camera.SetAspectRatio((float)resizeEvent->GetWidth() / (float)resizeEvent->GetHeight());
            }

            Renderer& renderer = Renderer::Get();
            FrameBufferPtr renderBuffer = renderer.GetRenderBuffer();
            renderBuffer->Resize(resizeEvent->GetWidth(), resizeEvent->GetHeight());
            renderer.SetRenderBuffer(renderBuffer);

            break;
        }
    }
}

ScenePtr Application::GetMainScene() const
{
    return m_scene;
}

void Application::SetMainScene(const ScenePtr& scene)
{
    m_nextScene = scene;
}

void Application::SwitchScenes()
{
    m_scene = m_nextScene;
    Scripting::Engine& engine = Scripting::Engine::Get();
    GameManager& gameManager = GameManager::Get();
    
    engine.Clear();
    gameManager.Clear();
    
    for (const auto& entity : m_scene->Traverse())
    {
        Components::Scripted* script = entity.FindComponent<Components::Scriptable>();
        if (script)
        {
            engine.Register(script);
        }

        script = entity.FindComponent<Components::NavAgent>();
        if (script)
        {
            engine.Register(script);
        }

        script = entity.FindComponent<Components::Trigger>();
        if (script)
        {
            engine.Register(script);
        }

        Components::MonsterData* monster = entity.FindComponent<Components::MonsterData>();
        if (monster)
        {
            gameManager.AddMonster(entity);
        }
    }

    Entity mainCamera = m_scene->GetMainCamera();
    if (mainCamera)
    {
        mainCamera.GetComponent<Components::Camera>().camera.SetAspectRatio((float)m_window->GetWidth() / (float)m_window->GetHeight());
    }

    m_nextScene = nullptr;
}

