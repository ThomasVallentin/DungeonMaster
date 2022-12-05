#include "Engine.h"

#include "Scene/Components/Basics.h"

#include <algorithm>


namespace Scripting {
    

Engine Engine::s_instance;

Engine& Engine::Init()
{
    s_instance = Engine();
    return s_instance;
}

void Engine::Register(Components::Scripted* script)
{
    if (std::find(m_scripts.begin(), m_scripts.end(), script) == m_scripts.end())
    {
        m_scripts.push_back(script);
    }
}

void Engine::Deregister(Components::Scripted* script)
{
    auto it = std::find(m_scripts.begin(), m_scripts.end(), script);
    if (it != m_scripts.end())
    {
        m_scripts.erase(it);
    }
}

void Engine::OnUpdate()
{
    for (auto* script : m_scripts)
    {
        script->OnUpdate();
    }
}

void Engine::OnEvent(Event* event)
{
    for (auto* script : m_scripts)
    {
        script->OnEvent(event);
    }
}

void Engine::Clear()
{
    m_scripts.clear();
}

} // Namespace Scripting
