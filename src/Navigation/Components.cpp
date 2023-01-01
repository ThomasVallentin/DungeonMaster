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
    const Entity& entity = GetEntity();
    Transform* transform = entity.FindComponent<Transform>();
    if (!transform)
    {
        return;
    }

    if (m_agent->IsMoving() || m_agent->HasPath())
    {
        Transform* transform = GetEntity().FindComponent<Transform>();
        if (transform)
        {
            transform->transform = m_agent->GetNextTransform();
            m_agent->SetAdvanced();
        }
    }
    
    m_agent->SetTransform(Transform::ComputeWorldMatrix(entity));
}

Navigation::AgentPtr NavAgent::GetAgent() {
    return m_agent;
}


} // Namespace Components
