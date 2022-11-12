#version 460

in vec3 vLocalPos;
in vec3 vNormal;
in vec2 vTexCoords;
in float vDepth;

uniform sampler2D uTexture;
uniform vec3 uLightDirection = vec3(0.2, -0.5, -0.5);

out vec4 fFragColor;


vec3 exponentialFog(vec3 baseColor, float distance) 
{
    float fogAmount = 1.0 - exp( -distance * 0.0003 );
    vec3 fogColor = vec3(0.5, 0.6, 0.7);
    return mix(baseColor, fogColor, fogAmount);
}


void main() 
{
    vec3  color = texture(uTexture, vTexCoords).rgb;
    float directLight = max(0.0, dot(vNormal, -normalize(uLightDirection)));
    color *= vec3(directLight);
    color = color + vec3(0.05, 0.05, 0.1);

    fFragColor = vec4(exponentialFog(color, vDepth), 1.0);
}
