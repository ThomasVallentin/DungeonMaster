#ifndef AGENT_H
#define AGENT_H

#include "Core/Foundations.h"
#include "Core/Animation.h"

#include <glm/glm.hpp>


namespace Navigation {


class Engine;
class Agent;

DECLARE_PTR_TYPE(Agent);


class Agent
{
public:
    ~Agent();

    inline const glm::mat4& GetTransform() const { return m_transform; }
    void SetTransform(const glm::mat4& transform);

    inline const glm::vec3& GetDestination() const { return m_destination; }
    void SetDestination(const glm::vec3& destination);

    inline const float& GetSpeed() const { return m_speed; }
    void SetSpeed(const float& speed) { m_speed = speed; }

    inline void SetAdvanced() { m_hasAdvanced = true; }
    inline bool HasAdvanced() { return m_hasAdvanced; }
    inline glm::mat4 GetNextTransform() const { return m_nextTransform; }

    bool IsMoving() const;
    bool HasPath() const;

private:
    Agent();

    void SetPath(const std::vector<glm::vec2>& path);
    inline bool NeedsNewPath() const { return m_requestsNewPath; }

    void MakeProgress(const float& deltaTime);

    glm::vec3 m_destination;
    glm::mat4 m_nextTransform;
    glm::mat4 m_transform;
    float m_speed = 1.0f;
    Animation<glm::mat4> m_interpolator;

    std::vector<glm::vec2> m_path;

    bool m_hasAdvanced = false;
    bool m_requestsNewPath = false;

    friend Engine;
};


} // Namespace Navigation


#endif  // AGENT_H
