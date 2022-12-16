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
    Entity entity = script->GetEntity();
    if (entity.GetScene() != Application::Get().GetMainScene().get())
    {
        return;
    }

    auto it = m_scripts.find(entity);
    if (it == m_scripts.end())
    {
        it = m_scripts.insert({entity, {script}}).first;
    }

    if (std::find(it->second.begin(), it->second.end(), script) == it->second.end())
    {
        it->second.push_back(script);
    }
}

void Engine::Deregister(Components::Scripted* script)
{
    Entity entity = script->GetEntity();
    auto it = m_scripts.find(entity);
    if (it == m_scripts.end())
    {
        return;
    }

    auto scriptIt = std::find(it->second.begin(), it->second.end(), script);
    if (scriptIt != it->second.end())
    {
        // only "nullifying" the script when it is deregistered to avoid changing 
        // the amount of scripts while looping over them
        *scriptIt = nullptr; 
    }
}

void Engine::OnUpdate()
{
    for (auto& [entity, scripts] : m_scripts)
    {
        for (auto it = scripts.begin() ; it != scripts.end() ; )
        {
            if (!*it)
            {
                scripts.erase(it);
                continue;
            }
            
            (*it)->OnUpdate();
            it++;
        }
    }
}

void Engine::OnEvent(Event* event)
{
    for (auto& [entity, scripts] : m_scripts)
    {
        for (auto* script : scripts)
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
}

void Engine::Clear()
{
    m_scripts.clear();
}

} // Namespace Scripting
