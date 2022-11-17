#include "Application.h"

#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"

#include "Resources/Model.h"
#include "Resources/Manager.h"

#include "Resolver.h"
#include "Window.h"
#include "Event.h"
#include "Logging.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
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

    m_window = std::make_unique<Window>(WindowSettings{1280, 720, "Dungeon Master"});

    m_camera = Camera::Create(glm::lookAt(glm::vec3(0.0f, 70.0f, 120.0f), 
                                          glm::vec3(0.0f, 0.0f, 0.0f), 
                                          glm::vec3(0.0f, 1.0f, 0.0f)), 
                              {});
    m_renderBuffer = FrameBuffer::Create({ 1280, 720, 8 });
    m_scene = Scene::Create();
    Entity test = m_scene->CreateEntity("test");
    Entity test1 = m_scene->CreateEntity("test1", test);
    Entity test2 = m_scene->CreateEntity("test2", test1);
    Entity test3 = m_scene->CreateEntity("test3", test);
    Entity test4 = m_scene->CreateEntity("test4", test3);
    Entity test5 = m_scene->CreateEntity("test5", test3);

    for (Entity entity : m_scene->Traverse())
    {
        LOG_INFO("Entity :  !");
    }
}


void Application::Run()
{
    m_isRunning = true;

    auto& resolver = Resolver::Get(); 

    ResourceManager::LoadScene(resolver.Resolve("Levels/Labyrinth.ppm"));
    ResourceHandle<Mesh> mesh = ResourceManager::GetResource<Mesh>("Levels/Labyrinth.ppm:Floor1");

    ShaderPtr shader = Shader::Open(resolver.Resolve("Shaders/default.vert"),
                                    resolver.Resolve("Shaders/default.frag"));

    auto layout = shader->GetUniformBlockDescription("MaterialInputs");
    for (const auto& attr : layout.uniforms)
    {
        LOG_INFO("%s, %u, %u, %u", attr.name.c_str(), attr.offset, attr.type, attr.count);
    }
    
    // ResourceHandle<Model> model = ResourceManager::LoadModel("Models/Japanese_Garden.fbx");
    ResourceHandle<Texture> texture = ResourceManager::LoadTexture("Textures/Checker.jpg");

    MaterialPtr material = Material::Create(shader);
    material->SetInputValue<glm::vec3>("diffuseColor", {1.0, 1.0, 1.0});
    material->SetInputTexture("diffuseColor", texture.Get()->GetId());
    
    MaterialPtr material2 = Material::Create(shader);
    material2->SetInputValue<glm::vec3>("diffuseColor", {0.2, 0.2, 0.5});

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while (m_isRunning)
    {
        double time = GetCurrentTime();

        // for (auto& [entity, script] : m_scene.Traverse<ScriptedComponent>())
        // {
        //     script.OnUpdate();
        // }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        m_renderBuffer->Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Update game here
        shader->SetMat4("uMVPMatrix", m_camera->GetViewProjMatrix());

        int i = 0;
        // for (const auto& mesh : model.Get()->GetMeshes())
        // {
        //     mesh.Get()->Bind();
        //     if (i%2)
        //     {
        //         material->Bind();
        //         material->ApplyUniforms();
        //     }
        //     else
        //     {
        //         material2->Bind();
        //         material2->ApplyUniforms();
        //     }

        //     glDrawElements(GL_TRIANGLES, 
        //                 mesh.Get()->GetElementCount(),
        //                 GL_UNSIGNED_INT,
        //                 nullptr);
        //     i++;
        // }
        material->Bind();
        material->ApplyUniforms();
        auto msh = mesh.Get();
        msh->Bind();

        glDrawElements(GL_TRIANGLES, 
                       msh->GetElementCount(),
                       GL_UNSIGNED_INT,
                       nullptr);

        m_camera->SetViewMatrix(glm::rotate(m_camera->GetViewMatrix(), 0.01f, glm::vec3(0, 1, 0)));
        m_renderBuffer->Blit(0, m_renderBuffer->GetWidth(), m_renderBuffer->GetHeight());
        m_renderBuffer->Unbind();

        m_window->OnUpdate();
        
        std::ostringstream titleStream;
        titleStream << std::fixed << std::setprecision(2);
        titleStream << "Dungeon Master | " << (GetCurrentTime() - time) * 1000 << "ms/frame";
        m_window->SetTitle(titleStream.str());
    }
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

    // for (auto& [entity, script] : m_scene.Traverse<ScriptedComponent>())
    // {
    //     script.OnEvent(event);
    // }
}

double Application::GetCurrentTime()
{
    return glfwGetTime();
}
