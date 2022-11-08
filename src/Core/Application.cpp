#include "Application.h"

#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"

#include "Window.h"
#include "Event.h"
#include "Logging.h"


Application* Application::s_instance = nullptr;


Application& Application::Init(int argc, char* argv[])
{
    if (Application::s_instance) {
        LOG_WARNING("Application already exists, cannot Init() it twice.");
        return *s_instance;
    }

    s_instance = new Application(argc, argv);

    std::string appPath = argv[0];
    s_instance->m_appRootPath = std::filesystem::canonical(appPath).remove_filename().parent_path().parent_path();

    return *s_instance;
}


Application::Application(int argc, char* argv[])
{
    m_window = std::make_unique<Window>(WindowSettings{1280, 720, "Dungeon Master"});
}


void Application::Run()
{
    m_isRunning = true;

    // VertexBuffer
    Vertex vertices[] = {{{-0.5, -0.5, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
                         {{-0.5,  0.5, 0.0}, {0.0, 0.0, 0.0}, {0.0, 1.0}},
                         {{ 0.5,  0.5, 0.0}, {0.0, 0.0, 0.0}, {1.0, 1.0}},
                         {{ 0.5, -0.5, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0}}
                        };
    VertexBufferPtr vtxBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
    vtxBuffer->SetLayout({{"aPosition",  3, GL_FLOAT, false},
                          {"aNormal",    3, GL_FLOAT, false},
                          {"aTexCoords", 2, GL_FLOAT, false}
                         });

    // IndexBuffer
    GLuint indices[] = {0, 1, 2,
                        2, 3, 0};
    IndexBufferPtr idxBuffer = IndexBuffer::Create(indices, 6); 

    VertexArrayPtr vtxArray = VertexArray::Create();
    vtxArray->AddVertexBuffer(vtxBuffer);
    vtxArray->SetIndexBuffer(idxBuffer);

    ShaderPtr shader = Shader::Open(GetResourcePath("Shaders/default.vert"),
                                    GetResourcePath("Shaders/default.frag"));

    TexturePtr texture = Texture::Open({GetResourcePath("Textures/Checker.jpg"), ColorSpace::Raw});
    LOG_INFO("texture is valid : %d", texture->IsValid());

    shader->Bind();
    texture->Bind(0);
    shader->SetInt("uTexture", 0);

    while (m_isRunning) 
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update game here
        vtxArray->Bind();
        shader->Bind();
        
        glDrawElements(GL_TRIANGLES, 
                       vtxArray->GetIndexBuffer()->GetCount(),
                       GL_UNSIGNED_INT,
                       nullptr);

        m_window->OnUpdate();
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
    }
}

std::string Application::GetResourcePath(const std::string& path)
{
    return Get().m_appRootPath / "resources" / path;
}
