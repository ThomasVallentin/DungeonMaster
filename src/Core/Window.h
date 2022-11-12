#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <functional>


class Event;
class GLFWwindow;


static bool s_glfwHasBeenInitialized = false;


struct WindowSettings {
    uint32_t width;
    uint32_t height;
    std::string title;
};


struct WindowInternalData {
    uint32_t width;
    uint32_t height;
    std::string title;
};


class Window
{
public:
    explicit Window(const WindowSettings& settings);
    ~Window();

    int GetWidth() const;
    int GetHeight() const;
    
    void OnUpdate();
    void SwapBuffer() const; 

    void SetTitle(const std::string& title);

private:
    void OnEvent(Event& event);

    static void ErrorHandler(int error, const char* description); 

    GLFWwindow* m_window;
    WindowInternalData m_internalData;
};

#endif
