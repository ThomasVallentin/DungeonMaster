#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include "Core/Event.h"

#include <vector>


namespace Components
{
    class Script;
}


class ScriptEngine
{
public:
    static ScriptEngine& Init();
    inline static ScriptEngine& Get() { return s_instance; }

    void Register(Components::Script* script);
    void Deregister(Components::Script* script);

    void OnUpdate();
    void OnEvent(Event* event);

    void Clear();

private:
    ScriptEngine() = default;
    ~ScriptEngine() = default;

    std::vector<Components::Script*> m_scripts;

    static ScriptEngine s_instance;
};

#endif  // SCRIPTENGINE_H