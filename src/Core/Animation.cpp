#include "Animation.h"


// Specialize matrix animation since they are not achieved through a simple linear blend
template <>
glm::mat4 Animation<glm::mat4>::Interpolate(const glm::mat4& first, 
                                            const glm::mat4& second, 
                                            const float& weight)
{ 
    return glm::interpolate(first, second, weight);
}
