#include "Components.h"

#include "Engine.h"

#include "Scene/Components/Basics.h"

#include <glm/gtx/string_cast.hpp>


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
        Transform* transform = GetEntity().FindComponent<Transform>();
        if (transform)
        {
            glm::vec3 pos = transform->GetPosition();
            m_agent->SetTransform(transform->transform);
        }
        
        return;
    }
    
    Transform* transform = GetEntity().FindComponent<Transform>();
    if (transform)
    {
        transform->transform = m_agent->GetNextTransform();
        m_agent->SetTransform(transform->transform);

        m_agent->SetAdvanced();
    }
}

Navigation::AgentPtr NavAgent::GetAgent() {
    return m_agent;
}


} // Namespace Components
