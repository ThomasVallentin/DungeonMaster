#include "Application.h"

#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Mesh.h"

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
                         {{0.0,   0.5, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
                         {{0.5,  -0.5, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0}}
                        };
    VertexBufferPtr vtxBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
    vtxBuffer->SetLayout({{"aPosition",  3, GL_FLOAT, false},
                          {"aNormal",    3, GL_FLOAT, false},
                          {"aTexCoords", 2, GL_FLOAT, false}
                         });

    // IndexBuffer
    GLuint indices[] = {0, 1, 2};
    IndexBufferPtr idxBuffer = IndexBuffer::Create(indices, 3); 

    VertexArrayPtr vtxArray = VertexArray::Create();
    vtxArray->AddVertexBuffer(vtxBuffer);
    vtxArray->SetIndexBuffer(idxBuffer);

    ShaderPtr shader = Shader::Open("/home/tvallentin/Projects/M2/Synthese_Image/DungeonMaster/build/resources/Shaders/default.vert",
                                    "/home/tvallentin/Projects/M2/Synthese_Image/DungeonMaster/build/resources/Shaders/default.frag");


    // auto sizof = sizeof(vertices);
    // GLuint vbo;
    // glGenBuffers(1, &vbo);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // GLuint ebo;
    // glGenBuffers(1, &ebo);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // GLuint vao;
    // glGenVertexArrays(1, &vao);
    // glBindVertexArray(vao);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 3));
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 5));
    // glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);
    // glEnableVertexAttribArray(2);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo);

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
