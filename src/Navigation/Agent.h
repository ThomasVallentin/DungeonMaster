#ifndef AGENT_H
#define AGENT_H

#include "Core/Foundations.h"
#include "Core/Animation.h"

#include <glm/glm.hpp>


class NavigationEngine;
class NavAgent;

DECLARE_PTR_TYPE(NavAgent);


class NavAgent
{
public:
    ~NavAgent();

    inline const glm::vec3& GetPosition() const { return m_position; }
    void SetPosition(const glm::vec3& position);

    inline const glm::vec3& GetDestination() const { return m_destination; }
    void SetDestination(const glm::vec3& destination);

    inline const float& GetSpeed() const { return m_speed; }
    void SetSpeed(const float& speed) { m_speed = speed; }

    inline void SetAdvanced() { m_hasAdvanced = true; }
    inline bool HasAdvanced() { return m_hasAdvanced; }
    inline const glm::vec3& GetNextPosition() const { return m_nextPosition; }

    bool IsMoving() const;

private:
    NavAgent();

    void SetPath(const std::vector<glm::vec2>);
    inline bool NeedsNewPath() const { return m_requestsNewPath; }

    void MakeProgress(const float& deltaTime);

    glm::vec3 m_position;
    glm::vec3 m_destination;
    glm::vec3 m_nextPosition;
    float m_speed = 1.0f;
    Animation<glm::vec3> m_interpolator;

    std::vector<glm::vec2> m_path;

    bool m_hasAdvanced = false;
    bool m_requestsNewPath = false;

    friend NavigationEngine;
};

#endif  // AGENT_H
