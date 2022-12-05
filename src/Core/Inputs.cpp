#include "Inputs.h"

#include "Core/Window.h"
#include "Core/Application.h"

#include <GLFW/glfw3.h>


bool Inputs::IsKeyPressed(const KeyCode &key)
{
    int state = glfwGetKey(Application::Get().GetWindow().GetInternalWindow(), (int)key);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Inputs::IsMouseButtonPressed(const MouseButton &button)
{
    int state = glfwGetMouseButton(Application::Get().GetWindow().GetInternalWindow(), (int)button);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

glm::vec2 Inputs::GetMousePosition()
{
    double x, y;
    glfwGetCursorPos(Application::Get().GetWindow().GetInternalWindow(), &x, &y);
    return glm::vec2((float)x, (float)y);
}
