#include "Window.h"

#include "Logging.h"
#include "Event.h"
#include "Time.h"
#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


Window::Window(const WindowSettings& settings) :
        m_internalData({settings.width, 
                        settings.height,
                        settings.title.c_str()})
{
    if (!s_glfwHasBeenInitialized) {
        
        if (!glfwInit()) {
            LOG_ERROR("Something went wrong trying to initialize GLFW !");
            return;
        }
        
        glfwSetErrorCallback(Window::ErrorHandler);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);          

        s_glfwHasBeenInitialized = true;
    }

    m_window = glfwCreateWindow(settings.width, settings.height, settings.title.c_str(), nullptr, nullptr);
    if (!m_window) {
        LOG_ERROR("Something went wrong trying to open a GLFW window !");
        return;
    }
    glfwMakeContextCurrent(m_window);

    // Enable VSync
    glfwSwapInterval(1);

    // Pass internal data to the window
    glfwSetWindowUserPointer(m_window, &m_internalData);

    // Define callbacks
    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
    {
        WindowClosedEvent event;
        Application::Get().EmitEvent(&event);
    });

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
    {
        WindowInternalData& internalData = *(WindowInternalData*)glfwGetWindowUserPointer(window);
        internalData.width = width;
        internalData.height = height;

        WindowResizedEvent event(width, height);
        Application::Get().EmitEvent(&event);
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
    {
        MouseMovedEvent event(xpos, ypos);
        Application::Get().EmitEvent(&event);
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset)
    {
        MouseScrolledEvent event(xoffset, yoffset);
        Application::Get().EmitEvent(&event);
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
    {
        switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event((MouseButton)button, mods);
                Application::Get().EmitEvent(&event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event((MouseButton)button, mods);
                Application::Get().EmitEvent(&event);
                break;
            }
        }
    });

    glfwSetKeyCallback(m_window,[](GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        switch (action) {
            case GLFW_PRESS:
            case GLFW_REPEAT: {
                KeyPressedEvent event((KeyCode)key, mods);
                Application::Get().EmitEvent(&event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event((KeyCode)key, mods);
                Application::Get().EmitEvent(&event);
                break;
            }
        }
    });

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return;

    LOG_DEBUG("Successfully loaded OpenGL !\n"
              "            Vendor: %s\n"
              "            Renderer: %s\n"
              "            Version: %s", 
              glGetString(GL_VENDOR), 
              glGetString(GL_RENDERER), 
              glGetString(GL_VERSION));

    // Add GL callbacks
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GlErrorHandler, 0);

    // OpenGL global configuration
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    m_window = nullptr;
}

int Window::GetWidth() const
{
    WindowInternalData& internalData = *(WindowInternalData*)glfwGetWindowUserPointer(m_window);
    return internalData.width;
}

int Window::GetHeight() const
{
    WindowInternalData& internalData = *(WindowInternalData*)glfwGetWindowUserPointer(m_window);
    return internalData.height;
}

void Window::OnUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(m_window);
}

void Window::SetTitle(const std::string& title)
{
    glfwSetWindowTitle(m_window, title.c_str());
}

double Window::GetInternalTime() const 
{
    return glfwGetTime();
}

void Window::ErrorHandler(int error, const char* description)
{
    LOG_ERROR("Glfw error %d: %s", error, description);
}

void Window::GlErrorHandler(uint32_t source,
                            uint32_t type,
                            uint32_t id,
                            uint32_t severity,
                            int length,
                            const char* message,
                            const void* userParam )
{
    if (severity == GL_DEBUG_TYPE_ERROR)
        LOG_ERROR("GL ERROR : Type = 0x%x, Severity = 0x%x, Message = %s",
                type, severity, message);

#ifdef ENABLE_DEBUG
    else
        LOG_DEBUG("GL : Type = 0x%x, Severity = 0x%x, Message = %s",
                  type, severity, message);
#endif // ENABLE_DEBUG

}
