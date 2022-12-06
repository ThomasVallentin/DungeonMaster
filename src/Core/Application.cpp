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
    Resolver::Init(std::filesystem::canonical(appPath).remove_filename().parent_path().parent_path());

    Scripting::Engine::Init();
    Navigation::Engine::Init();

    m_window = std::make_unique<Window>(WindowSettings{1280, 720, "Dungeon Master"});
    m_renderBuffer = FrameBuffer::Create({ 1280, 720, 8 });
    m_scene = Scene::Create();   
}

void Application::Run()
{
    m_isRunning = true;

    auto& resolver = Resolver::Get(); 

    m_scene = ResourceManager::LoadLevel("Levels/Labyrinth.json").Get();
    m_scene->GetMainCamera().GetComponent<Components::Camera>().camera.SetAspectRatio((float)m_window->GetWidth() / (float)m_window->GetHeight());

    Navigation::Engine& navEngine = Navigation::Engine::Get();
    navEngine.SetNavMap(Image::Read(resolver.Resolve("Levels/Labyrinth.ppm")));

    glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
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
    double time = GetCurrentTime();

    // Update Navigation
    Navigation::Engine& navEngine = Navigation::Engine::Get();
    navEngine.OnUpdate();

    // Updating Scripts
    Scripting::Engine::Get().OnUpdate();

    // Rendering part, should be handled by a separated Renderer class
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_renderBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Update game here
    Entity cameraEntity = m_scene->GetMainCamera();
    auto* camera = cameraEntity.FindComponent<Components::Camera>();
    if (!camera)
    {
        m_renderBuffer->Unbind();
        return;
    }

    glm::mat4 viewMatrix = glm::inverse(Components::Transform::ComputeWorldMatrix(cameraEntity));
    glm::mat4 projMatrix = camera->camera.GetProjMatrix();
    glm::mat4 viewProjMatrix = projMatrix * viewMatrix;

    for (Entity entity : m_scene->Traverse())
    {
        auto* meshComp = entity.FindComponent<Components::Mesh>();
        auto* meshRenderComp = entity.FindComponent<Components::RenderMesh>();
        
        if (meshComp && meshRenderComp)
        {
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
            material->GetShader()->SetVec3("uPointLight.position", glm::vec3(glm::inverse(viewMatrix) * glm::vec4(0, 0, 0, 1)));  // Flicking torch effect
            material->GetShader()->SetVec3("uPointLight.color", glm::vec3(0.8 + (std::abs(sin(time * 2.3)) * 2 + sin(0.5 + time * 7.7)) * 0.3) * 3.0f);  // Flicking torch effect
            material->GetShader()->SetFloat("uTime", time); 
            mesh->Bind();

            glDrawElements(GL_TRIANGLES, 
                            mesh->GetElementCount(),
                            GL_UNSIGNED_INT,
                            nullptr);
        }

    }

    m_renderBuffer->Blit(0, m_renderBuffer->GetWidth(), m_renderBuffer->GetHeight());
    m_renderBuffer->Unbind();
}

void Application::OnEvent(Event* event)
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
            break;
        }
    }

    // Sending events to the scripted components
    Scripting::Engine::Get().OnEvent(event);
}

double Application::GetCurrentTime()
{
    return glfwGetTime();
}
