#ifndef AGENT_H
#define AGENT_H

#include "Core/Foundations.h"
#include "Core/Animation.h"

#include <glm/glm.hpp>


class NavigationEngine;
class Agent;

DECLARE_PTR_TYPE(Agent);


enum class NavAgentType
{
    None = 0,
    Player,
    Monster
};


class Agent
{
public:
    ~Agent();

    inline const glm::vec3& GetPosition() const { return m_position; }
    void SetPosition(const glm::vec3& position);

    inline const glm::vec3& GetDestination() const { return m_destination; }
    void SetDestination(const glm::vec3& destination);

    inline const glm::vec3& GetNextPosition() const { return m_nextPosition; }

    inline bool IsMoving() const { return !m_animation.ended; }

private:
    Agent();

    glm::vec3 m_position;
    glm::vec3 m_destination;
    glm::vec3 m_nextPosition;
    Animation<glm::vec3> m_animation;

    friend NavigationEngine;
};

#endif  // AGENT_H
