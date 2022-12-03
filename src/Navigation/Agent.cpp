#include "Agent.h"

#include "NavigationEngine.h"


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
}

