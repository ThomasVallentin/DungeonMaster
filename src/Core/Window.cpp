#include "Window.h"

#include "Logging.h"
#include "Event.h"
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

        s_glfwHasBeenInitialized = true;
    }

    m_window = glfwCreateWindow(settings.width, settings.height, settings.title.c_str(), nullptr, nullptr);
    if (!m_window) {
        LOG_ERROR("Something went wrong trying to open a GLFW window !");
        return;
    }
    glfwMakeContextCurrent(m_window);

    // Pass internal data to the window
    glfwSetWindowUserPointer(m_window, &m_internalData);

    // Define callbacks
    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
    {
        WindowClosedEvent event;
        Application::Get().OnEvent(&event);
    });

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
    {
        WindowInternalData& internalData = *(WindowInternalData*)glfwGetWindowUserPointer(window);
        internalData.width = width;
        internalData.height = height;

        WindowResizedEvent event(width, height);
        Application::Get().OnEvent(&event);
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
    {
        MouseMovedEvent event(xpos, ypos);
        Application::Get().OnEvent(&event);
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset)
    {
        MouseScrolledEvent event(xoffset, yoffset);
        Application::Get().OnEvent(&event);
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
    {
        switch (action) {
            case GLFW_PRESS: {
                MouseButtonReleasedEvent event(button, mods);
                Application::Get().OnEvent(&event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button, mods);
                Application::Get().OnEvent(&event);
                break;
            }
        }
    });

    glfwSetKeyCallback(m_window,[](GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        switch (action) {
            case GLFW_PRESS:
            case GLFW_REPEAT: {
                MouseButtonReleasedEvent event(key, mods);
                Application::Get().OnEvent(&event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(key, mods);
                Application::Get().OnEvent(&event);
                break;
            }
        }
    });

    // Initialize GLAD
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    LOG_DEBUG("Successfully loaded OpenGL !\n"
              "            Vendor: %s\n"
              "            Renderer: %s\n"
              "            Version: %s", 
              glGetString(GL_VENDOR), 
              glGetString(GL_RENDERER), 
              glGetString(GL_VERSION));

    // Enable VSync
    glfwSwapInterval(1);

    // Enabling depth
    glEnable(GL_DEPTH_TEST);
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    m_window = nullptr;
}

inline int Window::GetWidth() const
{
    WindowInternalData& internalData = *(WindowInternalData*)glfwGetWindowUserPointer(m_window);
    return internalData.width;
}

inline int Window::GetHeight() const
{
    WindowInternalData& internalData = *(WindowInternalData*)glfwGetWindowUserPointer(m_window);
    return internalData.height;
}

void Window::OnUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(m_window);
}

void Window::ErrorHandler(int error, const char* description)
{
    LOG_ERROR("Glfw error %d: %s", error, description);
}

