#version 460 core

// == INPUTS ==

in vec2 vTexCoords;

// == UNIFORMS ==

uniform sampler2D uTexture;

// == OUTPUTS ==

out vec4 fFragColor;

void main()
{
    fFragColor = vec4(texture(uTexture, vTexCoords).rgb, 1.0);
    // fFragColor = vec4(vTexCoords, 1.0, 1.0);
}
