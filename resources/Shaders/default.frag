#version 460

in vec3 vLocalPos;
in vec3 vNormal;
in vec2 vTexCoords;
in float vDepth;

layout(std140, binding = 0) uniform MaterialInputs
{
    vec3  diffuseColor;
    bool  diffuseColorUseTexture;

    vec3  ambientColor;
    bool  ambientColorUseTexture;
};


uniform sampler2D uTextures[2];
uniform vec3 uLightDirection = vec3(0.2, -0.5, -0.5);


out vec4 fFragColor;


const int diffuseColorTexture = 0;
const int ambientColorTexture = 1;


vec3 SampleDiffuse()
{
    return mix(diffuseColor, 
               texture(uTextures[diffuseColorTexture], vTexCoords).rgb,
               float(diffuseColorUseTexture));
}

vec3 SampleAmbient()
{
    return mix(diffuseColor, 
               texture(uTextures[ambientColorTexture], vTexCoords).rgb, 
               float(diffuseColorUseTexture));
}

// vec3 exponentialFog(vec3 baseColor, float distance) 
// {
//     float fogAmount = 1.0 - exp( -distance * 0.0003 );
//     vec3 fogColor = vec3(0.5, 0.6, 0.7);
//     return mix(baseColor, fogColor, fogAmount);
// }


void main() 
{
    fFragColor = vec4(SampleDiffuse(), 1.0);
    

    // fFragColor = vec4(exponentialFog(color, vDepth), 1.0);
}
