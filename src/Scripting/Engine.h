#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include "Components.h"

#include "Core/Event.h"

#include <vector>


namespace Scripting {


class Engine
{
public:
    static Engine& Init();
    inline static Engine& Get() { return s_instance; }

    // TODO: Prevent Scripts that do not belong to the active scene from beeing added to the Engine
    void Register(Components::Scripted* script);
    void Deregister(Components::Scripted* script);

    void OnUpdate();
    void OnEvent(Event* event);

    void Clear();

private:
    Engine() = default;
    ~Engine() = default;

    std::vector<Components::Scripted*> m_scripts;

    static Engine s_instance;
};

} // Namespace Scripting

#endif  // SCRIPTENGINE_H