#include "Application.h"

#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"

#include "Scene/Entity.h"
#include "Scene/Components/Basics.h"
#include "Scene/Components/Game.h"

#include "Resources/Model.h"
#include "Resources/Manager.h"

#include "Resolver.h"
#include "Window.h"
#include "Event.h"
#include "Logging.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>

#include <sstream>


Application* Application::s_instance = nullptr;


struct Xform 
{
    Xform(const glm::mat4& mat) : xform(mat) {
        LOG_INFO("Bim ! Alive !!!");
    }

    ~Xform() {
        LOG_INFO("AAAAAAAAaaarggggg !!!");
    }
    
    glm::mat4 xform;
};


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

    ScriptEngine::Init();

    m_window = std::make_unique<Window>(WindowSettings{1280, 720, "Dungeon Master"});
    m_camera = Camera::Create(glm::lookAt(glm::vec3(10.0f, 0.5f, 4.0f), 
                                          glm::vec3(10.0f, 0.5f, 0.0f), 
                                          glm::vec3(0.0f, 1.0f, 0.0f)), 
                              {});
    m_renderBuffer = FrameBuffer::Create({ 1280, 720, 8 });
    m_scene = Scene::Create();   
}


void Application::Run()
{
    m_isRunning = true;

    auto& resolver = Resolver::Get(); 

    m_scene = ResourceManager::LoadLevel("Levels/Labyrinth.json").Get();

    // Character controller
    Entity player = m_scene->CreateEntity("Player");
    player.EmplaceComponent<Components::Transform>();
    auto& controller = player.EmplaceComponent<Components::Script>(Components::CreateCharacterController(player));
    
    // for (Entity entity : m_scene.Get()->Traverse())
    // {
    //     LOG_INFO("Entity : %s", entity.GetName().c_str());
    // }

    glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
    while (m_isRunning)
    {
        double time = GetCurrentTime();

        OnUpdate();
        m_window->OnUpdate();
        
        std::ostringstream titleStream;
        titleStream << std::fixed << std::setprecision(2);
        titleStream << "Dungeon Master | " << (GetCurrentTime() - time) * 1000 << "ms/frame";
        m_window->SetTitle(titleStream.str());
    }
}

void Application::OnUpdate() 
{
    double time = GetCurrentTime();

    // Updating Scripts
    ScriptEngine::Get().OnUpdate();

    // Rendering part, should be handled by a separated Renderer class
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_renderBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Update game here
    for (Entity entity : m_scene->Traverse())
    {
        auto* meshComp = entity.FindComponent<Components::Mesh>();
        auto* meshRenderComp = entity.FindComponent<Components::RenderMesh>();
        
        if (meshComp && meshRenderComp)
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
                
            Entity parent = entity;
            while (parent)
            {
                if (auto* transform = parent.FindComponent<Components::Transform>())
                {
                    modelMatrix = transform->transform * modelMatrix;
                }
                parent = parent.GetParent();
            }

            auto material = meshRenderComp->material.Get();
            auto mesh = meshComp->mesh.Get();

            // TODO: Insert culling here

            material->Bind();
            material->ApplyUniforms();
            material->GetShader()->SetMat4("uModelViewMatrix", m_camera->GetViewMatrix() * modelMatrix);
            material->GetShader()->SetMat3("uNormalMatrix", glm::transpose(glm::inverse(glm::mat3(m_camera->GetViewMatrix()) * glm::mat3(modelMatrix))));
            material->GetShader()->SetMat4("uMVPMatrix", m_camera->GetViewProjMatrix() * modelMatrix);
            material->GetShader()->SetVec3("uPointLight.color", glm::vec3(0.8 + (std::abs(sin(time * 2.3)) * 2 + sin(0.5 + time * 7.7)) * 0.3));  // Flicking torch effect
            mesh->Bind();

            glDrawElements(GL_TRIANGLES, 
                            mesh->GetElementCount(),
                            GL_UNSIGNED_INT,
                            nullptr);
        }

    }

    m_camera->SetViewMatrix(m_camera->GetViewMatrix());
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
            ProjectionSpecs projSpecs = m_camera->GetProjectionSpecs();

            m_camera->SetAspectRatio((float)resizeEvent->GetWidth() / (float)resizeEvent->GetHeight());
            m_renderBuffer->Resize(resizeEvent->GetWidth(), resizeEvent->GetHeight());
            break;
        }
    }

    // Sending events to the scripted components
    ScriptEngine::Get().OnEvent(event);
}

double Application::GetCurrentTime()
{
    return glfwGetTime();
}
