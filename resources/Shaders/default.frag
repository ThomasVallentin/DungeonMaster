#version 460

in vec3 vLocalPos;
in vec3 vNormal;
in vec2 vTexCoords;

uniform sampler2D uTexture;
uniform vec3 uLightDirection = vec3(0.2, -0.5, -0.5);

out vec4 fFragColor;

void main() 
{
    fFragColor = vec4(vec3(max(0, dot(vNormal, -normalize(uLightDirection)))) + vec3(0.20, 0.20, 0.25), 1.0);
}
