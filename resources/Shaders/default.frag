#version 460

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


// == HELPER FUNCTIONS ==

vec3 ODT_Gamma(vec3 color, float gamma)
{
    return pow(color, vec3(1.0 / gamma));
}

// From Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
vec3 Tonemap_ACESFilmic(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 AddExponentialFog(vec3 baseColor, float distance, float density) 
{
    float fogAmount = 1.0 - exp( -distance * density );
    vec3 fogColor = vec3(0.5, 0.6, 0.7);
    return fogColor * fogAmount + baseColor * (1.0 - fogAmount);
}


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
    // color = AddExponentialFog(color, vDepth, 0.03);

    // fFragColor = vec4(SampleDiffuse() * clamp(dot(-normalize(uLightDirection), normalize(vNormal)), 0, 1), 1.0);
    color = Tonemap_ACESFilmic(color);
    color = ODT_Gamma(color, 2.2);  // Pseudo sRGB ODT;

    fFragColor = vec4(color, 1.0);
}
