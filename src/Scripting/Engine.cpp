#include "Engine.h"

#include "Scene/Components/Basics.h"

#include "Core/Application.h"

#include <algorithm>


namespace Scripting {
    

Engine* Engine::s_instance = nullptr;

Engine& Engine::Init()
{
    s_instance = new Engine();
    return *s_instance;
}

void Engine::Register(Components::Scripted* script)
{
    if (script->GetEntity().GetScene() != Application::Get().GetMainScene().get())
    {
        return;
    }

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
        // only "nullifying" the script when it is deregistered to avoid changing 
        // the amount of scripts while looping over them
        *it = nullptr; 
    }
}

void Engine::OnUpdate()
{
    for (auto it = m_scripts.begin() ; it != m_scripts.end() ; )
    {
        if (!*it)
        {
            m_scripts.erase(it);
            continue;
        }
        
        (*it)->OnUpdate();
        it++;
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
