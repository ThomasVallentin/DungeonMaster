#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 uMVPMatrix = mat4(1.0);
uniform mat4 uModelMatrix = mat4(1.0);
uniform mat4 uNormalMatrix = mat4(1.0);

out vec3 vLocalPos;
out vec3 vNormal;
out vec2 vTexCoords;

void main()
{
    vLocalPos = (uModelMatrix * vec4(aPosition, 1.0)).xyz;
    vNormal = (uNormalMatrix * vec4(aNormal, 0.0)).xyz;
    vTexCoords = aTexCoords;

    gl_Position = vec4(aPosition, 1.0);
}
