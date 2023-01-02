#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 uMVPMatrix = mat4(1.0);
uniform mat4 uModelMatrix = mat4(1.0);
uniform mat4 uViewMatrix = mat4(1.0);
uniform mat4 uCameraModelMatrix = mat4(1.0);
uniform mat3 uNormalMatrix = mat3(1.0);
uniform bool uDoubleSided = false;

out vec3 vWorldPos;
out vec3 vVertexToCam;
out vec3 vNormal;
out vec2 vTexCoords;
out float vDepth;

void main()
{
    vWorldPos = (uModelMatrix * vec4(aPosition, 1.0)).xyz;
    
    vVertexToCam = uCameraModelMatrix[3].xyz - vWorldPos;
    vDepth = length(vVertexToCam);
    vVertexToCam /= vDepth;
    
    // Flipping normals if doubleSided is requested
    vNormal = uNormalMatrix * aNormal;
    if (uDoubleSided && (dot(vNormal, vVertexToCam) < 0.0))
    {
        vNormal *= -1.0;
    }

    vTexCoords = aTexCoords;
    
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
}
