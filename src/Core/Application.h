#ifndef APPLICATION_H
#define APPLICATION_H

#include "Foundations.h"

#include "Renderer/FrameBuffer.h"
#include "Renderer/Camera.h"

#include <filesystem>


class Window;
class Event;


class Application
{
public:
    static Application& Init(int argc, char* argv[]);
    inline static Application& Get() { return *s_instance; };

    void Run();
    void OnUpdate();
    void OnEvent(Event* event);
    inline int GetExitCode() const { return m_exitCode; }

    inline Window& GetWindow() const { return *m_window; };

    static std::string GetResourcePath(const std::string& path);
    static double GetCurrentTime();

private:
    Application(int argc, char* argv[]);
    ~Application() = default;

private:
    std::filesystem::path m_appRootPath;
    uint32_t m_exitCode = 0;

    std::unique_ptr<Window> m_window;
    bool m_isRunning;

    CameraPtr m_camera;
    FrameBufferPtr m_renderBuffer;

    static Application* s_instance;
};


#endif  // APPLICATION_H