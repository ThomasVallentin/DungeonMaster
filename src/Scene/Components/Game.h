#ifndef GAMECOMPONENTS_H
#define GAMECOMPONENTS_H

#include "Basics.h"

#include "Core/Event.h"
#include "Core/Application.h"

#include <GLFW/glfw3.h>

#include <map>


namespace Components {

Scriptable CreateCharacterController(const Entity& entity);

class NavAgent : public Scripted
{
public:
    NavAgent(const std::string& name) : Scripted(name) {}
};

} // Namespace Components


#endif // GAMECOMPONENTS_H