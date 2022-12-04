#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include "Core/Event.h"

#include <vector>


namespace Components
{
    class Scripted;
}


class ScriptEngine
{
public:
    static ScriptEngine& Init();
    inline static ScriptEngine& Get() { return s_instance; }

    // TODO: Prevent Scripts that do not belong to the active scene from beeing added to the Engine
    void Register(Components::Scripted* script);
    void Deregister(Components::Scripted* script);

    void OnUpdate();
    void OnEvent(Event* event);

    void Clear();

private:
    ScriptEngine() = default;
    ~ScriptEngine() = default;

    std::vector<Components::Scripted*> m_scripts;

    static ScriptEngine s_instance;
};

#endif  // SCRIPTENGINE_H