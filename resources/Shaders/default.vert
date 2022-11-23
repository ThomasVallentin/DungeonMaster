#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 uMVPMatrix = mat4(1.0);
uniform mat4 uModelViewMatrix = mat4(1.0);
uniform mat3 uNormalMatrix = mat3(1.0);

out vec3 vViewPos;
out vec3 vNormal;
out vec2 vTexCoords;
out float vDepth;

void main()
{
    vViewPos = (uModelViewMatrix * vec4(aPosition, 1.0)).xyz;
    vDepth = length(vViewPos);
    vNormal = uNormalMatrix * aNormal;
    vTexCoords = aTexCoords;
    
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
}
