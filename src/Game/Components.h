#ifndef GAMECOMPONENTS_H
#define GAMECOMPONENTS_H

#include "Scene/Components/Basics.h"

#include "Navigation/Agent.h"


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
    Navigation::AgentPtr m_agent;
};

} // Namespace Components


#endif // GAMECOMPONENTS_H