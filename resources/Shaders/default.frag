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


struct PointLight
{
    vec3 position;
    vec3 color;
    float decay;
};

uniform PointLight uPointLight = {vec3(0.0), vec3(1.0), 2.0};


// == HELPER FUNCTIONS ==

vec3 SamplePointLight(PointLight light, vec3 pos , vec3 normal)
{
    vec3 lightDir = light.position - pos;
    float distance = length(lightDir);

    return light.color / pow(distance, light.decay) * clamp(dot(normalize(lightDir), vNormal), 0, 1);
    
    // return vec3(0.0);
}

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
    vec3 color = SampleDiffuse();
    vec3 lighting = SamplePointLight(uPointLight, vViewPos, vNormal);

    color *= lighting;
    // color = exponentialFog(color, vDepth, 0.03);

    // fFragColor = vec4(SampleDiffuse() * clamp(dot(-normalize(uLightDirection), normalize(vNormal)), 0, 1), 1.0);
    
    fFragColor = vec4(pow(color, vec3(1/2.2)), 1.0);
}
