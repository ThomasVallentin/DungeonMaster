#ifndef NAVIGATIONCOMPONENTS_H
#define NAVIGATIONCOMPONENTS_H

#include "Agent.h"

#include "Scripting/Components.h"


namespace Components {


class NavAgent : public Scripted
{
public:
    NavAgent(const Entity& entity);
    NavAgent(const NavAgent& other);
    ~NavAgent();

    void OnUpdate() override;

    Navigation::AgentPtr GetAgent();

private:
    Navigation::AgentPtr m_agent;
};


}

#endif  // NAVIGATIONCOMPONENTS_H