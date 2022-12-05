#include "Agent.h"

#include "Engine.h"


namespace Navigation {


Agent::Agent()
{

}

Agent::~Agent()
{

}

void Agent::SetPosition(const glm::vec3& position)
{
    m_position = position;
}

void Agent::SetDestination(const glm::vec3& destination)
{
    m_destination = destination;
    m_requestsNewPath = true;
}

bool Agent::IsMoving() const
{
    return (!m_interpolator.ended && m_path.empty());
}

void Agent::MakeProgress(const float& deltaTime) 
{
    if (!m_interpolator.ended)
    {
        m_nextPosition = m_interpolator.Evaluate(deltaTime * m_speed);
        return;
    }

    if (m_path.size() > 1)
    {
        m_interpolator = Animation<glm::vec3>{{{0.0f, glm::vec3(m_path[0].x, 0.0f, m_path[0].y)},
                                               {1.0f, glm::vec3(m_path[1].x, 0.0f, m_path[1].y)}},
                                              InterpolationType::Smooth,
                                              1.0f};
        m_path.erase(m_path.begin());
        return;
    }

    m_nextPosition = m_position;
    m_path.clear();
}


} // Namespace Navigation 

