#version 460

in vec3 vLocalPos;
in vec3 vNormal;
in vec2 vTexCoords;

uniform sampler2D uTexture;

out vec4 fFragColor;

void main() 
{
    fFragColor = vec4(vLocalPos, 1.0);
}
