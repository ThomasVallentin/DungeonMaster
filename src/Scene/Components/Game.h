#ifndef GAMECOMPONENTS_H
#define GAMECOMPONENTS_H

#include "Basics.h"

#include "Navigation/Agent.h"

#include <GLFW/glfw3.h>

#include <map>


namespace Components {

Scriptable CreateCharacterController(const Entity& entity);

class NavAgent : public Scripted
{
public:
    NavAgent(const Entity& entity);
    NavAgent(const NavAgent& other);
    ~NavAgent();

    void OnUpdate() override;

private:
    NavAgentPtr m_agent;
};

} // Namespace Components


#endif // GAMECOMPONENTS_H