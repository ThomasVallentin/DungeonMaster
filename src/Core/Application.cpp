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
#include <glm/gtx/euler_angles.hpp>
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
    player.EmplaceComponent<Components::Transform>(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f)));
    auto& controller = player.EmplaceComponent<Components::Script>(Components::CreateCharacterController(player));
    
    Entity camera = player.AddChild("Camera");
    auto& cam = camera.EmplaceComponent<Components::Camera>();
    cam.camera.SetAspectRatio((float)m_window->GetWidth() / (float)m_window->GetHeight());
    m_scene->SetMainCamera(camera);
    
    // Weapon
    auto sword = ResourceManager::LoadModel("Models/Sting-Sword.fbx");
    Entity weapon = m_scene->CopyEntity(sword.Get()->GetRootEntity(), "Weapon", player);
    Components::Transform& weaponTransform = weapon.GetComponent<Components::Transform>();
    weaponTransform.transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.15f, 0.0f, -0.5f)) *
        glm::eulerAngleXYZ((float)M_PI_4, 0.0f, (float)M_PI_4) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(0.01f)) 
    ;

    LOG_INFO("player : %s", glm::to_string(Components::Transform::ComputeWorldMatrix(player)).c_str());
    LOG_INFO("weapon : %s", glm::to_string(Components::Transform::ComputeWorldMatrix(camera)).c_str());

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
        // if (entity.GetName() == "Player")
        //     LOG_INFO("player : %s", glm::to_string(Components::Transform::ComputeWorldMatrix(entity)).c_str());
        
        // if (entity.GetName() == "Weapon")
        // {
        //     auto* weapontransform = entity.FindComponent<Components::Transform>();
        //     weapontransform->transform = glm::translate(weapontransform->transform, glm::vec3(0, 0, sin(time*10)*0.1));
        // }

        // if (entity.GetName() == "Parent")
        // {
        //     auto* transform = entity.FindComponent<Components::Transform>();
        //     transform->transform = glm::rotate(transform->transform, 0.01f, glm::vec3(0, 1, 0));
        // }

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
            material->GetShader()->SetVec3("uPointLight.color", glm::vec3(0.8 + (std::abs(sin(time * 2.3)) * 2 + sin(0.5 + time * 7.7)) * 0.3));  // Flicking torch effect
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
    ScriptEngine::Get().OnEvent(event);
}

double Application::GetCurrentTime()
{
    return glfwGetTime();
}
