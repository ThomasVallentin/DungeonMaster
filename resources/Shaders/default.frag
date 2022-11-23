#version 460

// == INPUTS ==

in vec3 vViewPos;
in vec3 vNormal;
in vec2 vTexCoords;
in float vDepth;


// == UNIFORM BUFFERS ==

layout(std140, binding = 0) uniform MaterialInputs
{
    vec3  diffuseColor;
    bool  diffuseColorUseTexture;

    vec3  ambientColor;
    bool  ambientColorUseTexture;
};


// == UNIFORMS ==

uniform sampler2D uTextures[2];
uniform vec3 uLightDirection = vec3(0.2, -0.5, -0.5);


// == OUTPUTS ==

out vec4 fFragColor;


// == CONSTANTS ==

const int diffuseColorTexture = 0;
const int ambientColorTexture = 1;


// == HELP FUNCTIONS ==

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
               float(ambientColorUseTexture));
}

vec3 exponentialFog(vec3 baseColor, float distance, float density) 
{
    float fogAmount = 1.0 - exp( -distance * density );
    vec3 fogColor = vec3(0.5, 0.6, 0.7);
    return fogColor * fogAmount + baseColor * (1.0 - fogAmount);
}


// == SHADER EVALUATION ==

void main() 
{
    // fFragColor = vec4(0.0, 0.0, 0.0, 1.0);
    // fFragColor = vec4(SampleDiffuse(), 1.0);
    fFragColor = vec4(SampleDiffuse() * abs(dot(-normalize(uLightDirection), normalize(vNormal))), 1.0);
    
    fFragColor = vec4(exponentialFog(fFragColor.rgb, vDepth, 0.03), 1.0);
    fFragColor = pow(fFragColor, vec4(1/2.2, 1/2.2, 1/2.2, 1.0));
}
