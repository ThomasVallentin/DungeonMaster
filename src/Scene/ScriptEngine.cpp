#include "ScriptEngine.h"

#include "Components/Basics.h"

#include <algorithm>

ScriptEngine ScriptEngine::s_instance;

ScriptEngine& ScriptEngine::Init()
{
    s_instance = ScriptEngine();
    return s_instance;
}

void ScriptEngine::Register(Components::Scripted* script)
{
    if (std::find(m_scripts.begin(), m_scripts.end(), script) == m_scripts.end())
    {
        m_scripts.push_back(script);
    }
}

void ScriptEngine::Deregister(Components::Scripted* script)
{
    auto it = std::find(m_scripts.begin(), m_scripts.end(), script);
    if (it != m_scripts.end())
    {
        m_scripts.erase(it);
    }
}

void ScriptEngine::OnUpdate()
{
    for (auto* script : m_scripts)
    {
        script->OnUpdate();
    }
}

void ScriptEngine::OnEvent(Event* event)
{
    for (auto* script : m_scripts)
    {
        script->OnEvent(event);
    }
}

void ScriptEngine::Clear()
{
    m_scripts.clear();
}