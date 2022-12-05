#include "Components.h"

#include "Engine.h"

#include "Scene/Components/Basics.h"


namespace Components {


// == NavAgent ==

NavAgent::NavAgent(const Entity& entity) :
        Scripted("NavAgent", entity),
        m_agent(Navigation::Engine::Get().CreateAgent())
{
    
}

NavAgent::NavAgent(const NavAgent& other) :
        Scripted(other),
        m_agent(Navigation::Engine::Get().CreateAgent())
{

}

NavAgent::~NavAgent()
{
    Navigation::Engine::Get().RemoveAgent(m_agent);
}

void NavAgent::OnUpdate()
{
    if (!m_agent->IsMoving())
    {
        auto transform = GetEntity().FindComponent<Transform>();
        if (transform)
        {
            m_agent->SetPosition(transform->transform[3]);
        }
        
        return;
    }
    
    auto transform = GetEntity().FindComponent<Transform>();
    if (transform)
    {
        auto nextPos = m_agent->GetNextPosition();

        transform->transform[3] = glm::vec4(nextPos, 1.0);
        m_agent->SetPosition(nextPos);

        m_agent->SetAdvanced();
    }
}

Navigation::AgentPtr NavAgent::GetAgent() {
    return m_agent;
}


} // Namespace Components
