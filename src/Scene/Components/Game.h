#ifndef GAMECOMPONENTS_H
#define GAMECOMPONENTS_H

#include "Basics.h"

#include "Core/Event.h"
#include "Core/Application.h"

#include <GLFW/glfw3.h>

#include <map>


namespace Components {

    Script CreateCharacterController(const Entity& entity);

} // Namespace Components


#endif // GAMECOMPONENTS_H