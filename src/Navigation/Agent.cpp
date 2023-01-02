#include "Agent.h"

#include "Engine.h"

#include "Core/Logging.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Navigation {


Agent::Agent()
{

}

Agent::~Agent()
{

}

void Agent::SetTransform(const glm::mat4& transform)
{
    m_transform = transform;
}

void Agent::SetDestination(const glm::vec3& destination)
{
    m_destination = destination;
    m_requestsNewPath = true;
}

void Agent::SetPath(const std::vector<glm::vec2>& path)
{
    m_path = path;
    m_requestsNewPath = false;
}

bool Agent::IsMoving() const
{
    return !m_interpolator.ended;
}

bool Agent::HasPath() const
{
    return !m_path.empty();
}

void Agent::MakeProgress(const float& deltaTime) 
{
    if (!m_interpolator.ended)
    {
        m_nextTransform = m_interpolator.Evaluate(deltaTime);
    }
    else if (m_path.size() > 1)
    {
        glm::vec3 dir = glm::normalize(glm::vec3(m_transform[2].x, 0.0, -m_transform[2].z));
        glm::vec3 nextDir = glm::normalize(glm::vec3(m_path[1].x - m_path[0].x, 0.0f, -(m_path[1].y - m_path[0].y)));
        bool cellIsFree = !Engine::Get().CellContainsAgent(m_path[1]);
        float facingThePath = glm::dot(dir, nextDir);
        if (facingThePath < 0.999999)  // There can be some imprecision due to the matrix interpolation
        {
            // Agent faces the wrong direction 
            // -> Orienting the character so that it faces the path
            m_interpolator = Animation<glm::mat4>{{{0.0f, m_transform},
                                                   {1.0f, m_transform * glm::toMat4(glm::rotation(nextDir, dir))}},
                                                  InterpolationType::Smooth,
                                                  (facingThePath > -0.00001) ? m_speed : (m_speed * 0.5f)};
            m_interpolator.Start();
            m_nextTransform = m_interpolator.Evaluate(deltaTime);
        }
        else if (cellIsFree)
        {
            // Moving the character to the next cell
            glm::mat4 nextTransform = m_transform;
            nextTransform[3].x = m_path[1].x;
            nextTransform[3].z = m_path[1].y;

            m_interpolator = Animation<glm::mat4>{{{0.0f, m_transform},
                                                   {1.0f, nextTransform}},
                                                  InterpolationType::Smooth,
                                                  m_speed};
            m_path.erase(m_path.begin());
            m_interpolator.Start();
            m_nextTransform = m_interpolator.Evaluate(deltaTime);
        }
    }
    else {
        m_nextTransform = m_transform;
        m_path.clear();
    }
}


} // Namespace Navigation 

