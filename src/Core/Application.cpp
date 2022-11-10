#include "Application.h"

#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"

#include "Window.h"
#include "Event.h"
#include "Logging.h"

#include <GLFW/glfw3.h>

#include <sstream>


Application* Application::s_instance = nullptr;


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
    m_appRootPath = std::filesystem::canonical(appPath).remove_filename().parent_path().parent_path();

    m_window = std::make_unique<Window>(WindowSettings{1280, 720, "Dungeon Master"});

    m_renderBuffer = FrameBuffer::Create({ 1280, 720, 1 });
}


void Application::Run()
{
    m_isRunning = true;

    // VertexBuffer
    Vertex vertices[] = {{{-0.5, -0.25, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
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

    TexturePtr texture = Texture::Create(512, 512, GL_RGBA8);

    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while (m_isRunning) 
    {
        double time = GetCurrentTime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        m_renderBuffer->Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Update game here
        vtxArray->Bind();
        shader->Bind();
        shader->SetInt("uTexture", 0);
        texture->Bind(0);

        glDrawElements(GL_TRIANGLES, 
                       vtxArray->GetIndexBuffer()->GetCount(),
                       GL_UNSIGNED_INT,
                       nullptr);

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
            m_renderBuffer->Resize(resizeEvent->GetWidth(), resizeEvent->GetHeight());
            break;
        }
    }
}

std::string Application::GetResourcePath(const std::string& path)
{
    return Get().m_appRootPath / "resources" / path;
}

double Application::GetCurrentTime()
{
    return glfwGetTime();
}
