#version 460 core

// == INPUTS ==

in vec3 vWorldPos;
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

struct PointLight
{
    vec3 position;
    vec3 color;
    float decay;
};

uniform PointLight uPointLight = {vec3(0.0), vec3(1.0), 2.0};


// == OUTPUTS ==

out vec4 fFragColor;


// == CONSTANTS ==

const int diffuseColorTexture = 0;
const int ambientColorTexture = 1;


// == MATERIAL SAMPLING FUNCTIONS ==

vec3 SamplePointLight(PointLight light, vec3 pos , vec3 normal)
{
    vec3 lightDir = light.position - pos;
    float distance = length(lightDir);

    return light.color / pow(distance + 1.0, light.decay) * clamp(dot(lightDir / distance, normal), 0, 1);
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


// == SHADER EVALUATION ==

void main() 
{
    // Return normals (utils)
    // fFragColor = vec4(normalize(vNormal), 1.0);
    // return;
    
    vec3 color = SampleDiffuse();
    vec3 lighting = SamplePointLight(uPointLight, vWorldPos, normalize(vNormal));

    color *= lighting;

    fFragColor = vec4(color, 1.0);
}
