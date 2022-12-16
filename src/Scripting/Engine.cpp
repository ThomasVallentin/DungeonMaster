#include "Engine.h"

#include "Trigger.h"
#include "Scene/Components/Basics.h"

#include "Core/Application.h"
#include "Core/Logging.h"

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
        if (!script)
        {
            continue;
        }
        switch (event->GetCategory())
        {
            case EventCategory::Game:
            {
                switch (event->GetType())
                {
                    case TriggerEnterEvent::TypeId:
                    case TriggerStayEvent::TypeId:
                    case TriggerExitEvent::TypeId:
                    {
                        auto* triggerEvent = dynamic_cast<TriggerEvent*>(event);
                        if (triggerEvent->GetTriggered() == script->GetEntity())
                            script->OnEvent(event);

                        break;
                    }
                }
                break;
            }

            default:
                script->OnEvent(event);
                break;
        }
    }
}

void Engine::Clear()
{
    m_scripts.clear();
}

} // Namespace Scripting
