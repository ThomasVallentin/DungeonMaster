#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include "Components.h"

#include "Game/GameEvents.h"

#include <vector>
#include <unordered_map>


namespace Scripting {


class Engine
{
public:
    static Engine& Init();
    inline static Engine& Get() { return *s_instance; }

    // TODO: Prevent Scripts that do not belong to the active scene from beeing added to the Engine
    void Register(Components::Scripted* script);
    void Deregister(Components::Scripted* script);

    void OnUpdate();
    void EmitGameEvent(GameEvent* event);

    void Clear();

private:
    Engine() = default;
    ~Engine() = default;

    std::unordered_map<Entity, std::vector<Components::Scripted*>> m_scripts;

    static Engine* s_instance;
};

} // Namespace Scripting

#endif  // SCRIPTENGINE_H