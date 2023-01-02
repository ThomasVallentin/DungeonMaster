#include "Application.h"

#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"

#include "Scene/Entity.h"
#include "Scene/Components/Basics.h"

#include "Navigation/Engine.h"
#include "Navigation/Agent.h"
#include "Navigation/Components.h"

#include "Scripting/Trigger.h"

#include "Resources/Model.h"
#include "Resources/Manager.h"

#include "Game/GameManager.h"

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
    m_renderBuffer = FrameBuffer::Create({ 1280, 720, 8, {GL_RGBA32F} });
    m_postProcessBuffer = FrameBuffer::Create({ 1280, 720, 1, {GL_RGBA32F} });
    m_scene = Scene::Create();   

    m_renderImageShader = Shader::Open(resolver.Resolve("Shaders/fullScreen.vert"), 
                                       resolver.Resolve("Shaders/sprite.frag"));
    m_postProcessShader = Shader::Open(resolver.Resolve("Shaders/fullScreen.vert"), 
                                       resolver.Resolve("Shaders/postProcess.frag"));
}

void Application::Run()
{
    m_isRunning = true;

    GameManager::Get().ShowTitleScreen();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

    // Clearing all the FrameBuffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    m_postProcessBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    m_renderBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 viewMatrix(1.0f);
    glm::mat4 projMatrix(1.0f);

    Entity cameraEntity = m_scene->GetMainCamera();
    auto* camera = cameraEntity.FindComponent<Components::Camera>();
    if (camera)
    {
        viewMatrix = glm::inverse(Components::Transform::ComputeWorldMatrix(cameraEntity));
        projMatrix = camera->camera.GetProjMatrix();
    }

    glm::mat4 viewProjMatrix = projMatrix * viewMatrix;

    // Rendering the scene
    for (Entity entity : m_scene->Traverse())
    {
        auto* meshRenderComp = entity.FindComponent<Components::RenderMesh>();
        if (meshRenderComp)
        {
            auto* meshComp = entity.FindComponent<Components::Mesh>();
            if (!meshComp)
            {
                continue;
            }
            
            glm::mat4 modelMatrix = Components::Transform::ComputeWorldMatrix(entity);

            auto material = meshRenderComp->material.Get();
            auto mesh = meshComp->mesh.Get();

            // TODO: Insert culling here

            material->Bind();
            material->ApplyUniforms();
            material->GetShader()->SetMat4("uViewMatrix", viewMatrix);
            material->GetShader()->SetMat4("uModelMatrix", modelMatrix);
            material->GetShader()->SetMat3("uNormalMatrix", glm::transpose(glm::inverse(glm::mat3(modelMatrix))));
            material->GetShader()->SetMat4("uMVPMatrix", viewProjMatrix * modelMatrix);
            material->GetShader()->SetVec3("uPointLight.position", glm::vec3(glm::inverse(viewMatrix) * glm::vec4(0, 0, 0, 1)));
            material->GetShader()->SetVec3("uPointLight.color", glm::vec3(0.8 + (std::abs(sin(time * 2.3)) * 2 + sin(0.5 + time * 7.7)) * 0.3) * 3.0f);  // Flicking torch effect
            material->GetShader()->SetFloat("uTime", time); 
            mesh->Bind();

            glDrawElements(GL_TRIANGLES, 
                           mesh->GetElementCount(),
                           GL_UNSIGNED_INT,
                           nullptr);

            mesh->Unbind();
            material->Unbind();
            continue;
        }

        auto* renderImage = entity.FindComponent<Components::RenderImage>();
        if (renderImage)
        {
            VertexArrayPtr varray = VertexArray::Create();
            varray->Bind();

            m_renderImageShader->Bind();
            renderImage->image.Get()->Bind(0);
            m_renderImageShader->SetInt("uTexture", 0);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            varray->Unbind();
            continue;
        }
    }

    // Blit the render to the "post processing" buffer (non-multisampled)
    m_renderBuffer->Blit(m_postProcessBuffer->GetId(), m_renderBuffer->GetWidth(), m_renderBuffer->GetHeight());
    m_renderBuffer->Unbind();

    // Applying post processing in the main frame buffer
    VertexArrayPtr varray = VertexArray::Create();
    varray->Bind();

    m_postProcessShader->Bind();
    Texture::BindFromId(m_postProcessBuffer->GetColorAttachmentId(0), 0);
    Texture::BindFromId(m_postProcessBuffer->GetDepthAttachmentId(), 1);
    m_postProcessShader->SetInt("uBeauty", 0);
    m_postProcessShader->SetInt("uDepth", 1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    varray->Unbind();
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

            m_renderBuffer->Resize(resizeEvent->GetWidth(), resizeEvent->GetHeight());
            m_postProcessBuffer->Resize(resizeEvent->GetWidth(), resizeEvent->GetHeight());
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

