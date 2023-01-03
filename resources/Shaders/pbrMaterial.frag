#version 460 core

#define PI 3.1415926535897932384626433832795

// == INPUTS ==

in vec3 vWorldPos;
in vec3 vNormal;
in vec3 vVertexToCam;
in vec2 vTexCoords;
in float vDepth;


// == UNIFORM BUFFERS ==

layout(std140, binding = 0) uniform MaterialInputs
{
    vec3  baseColor;
    bool  baseColorUseTexture;

    float metallic;
    bool  metallicUseTexture;

    float roughness;
    bool  roughnessUseTexture;
    
    vec3 transmissionColor;
    bool transmissionColorUseTexture;

    vec3  emissionColor;
    bool  emissionColorUseTexture;
};


// == CONSTANTS ==

const int baseColorTexture = 0;
const int metallicTexture = 1;
const int roughnessTexture = 2;
const int transmissionColorTexture = 3;
const int emissionColorTexture = 4;

const int pointLightCount = 1;

// == UNIFORMS ==

uniform sampler2D uTextures[5];

struct PointLight
{
    vec3 position;
    vec3 color;
    float decay;
};

uniform PointLight uPointLights[pointLightCount];


// == OUTPUTS ==

out vec4 fFragColor;


// == MATERIAL SAMPLING FUNCTIONS ==

struct MaterialSample
{
    vec3  baseColor;
    float metallic;
    float roughness;
    vec3  transmissionColor;
    vec3  emissionColor;
};

vec3 SampleBaseColor()
{
    return mix(baseColor, 
               texture(uTextures[baseColorTexture], vTexCoords).rgb,
               float(baseColorUseTexture));
}

float SampleMetallic()
{
    return mix(metallic, 
               texture(uTextures[metallicTexture], vTexCoords).r,
               float(metallicUseTexture));
}

float SampleRoughness()
{
    return mix(roughness, 
               texture(uTextures[roughnessTexture], vTexCoords).r,
               float(roughnessUseTexture));
}
vec3 SampleTransmissionColor()
{
    return mix(transmissionColor, 
               texture(uTextures[transmissionColorTexture], vTexCoords).rgb,
               float(transmissionColorUseTexture));
}

vec3 SampleEmissionColor()
{
    return mix(emissionColor, 
               texture(uTextures[emissionColorTexture], vTexCoords).rgb,
               float(emissionColorUseTexture));
}

MaterialSample SampleMaterial()
{
    return MaterialSample(
        SampleBaseColor(),
        SampleMetallic(),
        SampleRoughness(),
        SampleTransmissionColor(),
        SampleEmissionColor()
    );
}

// == BRDF UTILS FUNCTIONS ==

// Trowbridge-Reitz (GGX) normal distribution function - 1975/2007
float NDF_TrowbridgeReitz_GGX(float NdotH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH2 = NdotH * NdotH;

    float denum = NdotH2 * (a2 - 1.0) + 1.0;
    denum       = PI * denum * denum;

    return a2 / denum;
}

// Schlick-Beckmann (GGX) geometric shadowing
float G_Schlick_GGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = r*r / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

// Smith (based on Schlick-Beckmann) geometric shadowing
float G_SmithSchlick(float NdotV, float NdotL, float roughness) {
    float G_V = G_Schlick_GGX(NdotV, roughness);
    float G_L = G_Schlick_GGX(NdotL, roughness);

    return G_V * G_L;
}

// Schlick Fresnel
vec3 Fr_Schlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// == BRDF ==

// Cook-Torrance microfacet BRDF
vec3 BRDF_CookTorrance_Microfacet(vec3 L, vec3 V, vec3 N, MaterialSample matSample, vec3 lightRadiance) {
    vec3 H = normalize(L + V);
    float HdotN = max(dot(H, N), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    // Compute normal distribution
    float NDF = NDF_TrowbridgeReitz_GGX(NdotH, matSample.roughness);

    // Compute Geometric shadowing
    float G = G_SmithSchlick(NdotV, NdotL, matSample.roughness);

    // Compute Fresnel
    vec3 F0 = mix(vec3(0.04), matSample.baseColor, matSample.metallic);
    vec3 F = Fr_Schlick(HdotN, F0);

    // Computing diffuse factor from fresnel and metallic
    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - matSample.metallic;

    // Computing the specular part of the BRDF
    float denum = 4.0 * NdotL * NdotV + 0.00001;  // Adding small constant to avoid dividing by zero;
    vec3 cookTorranceSpecular = NDF * G * F / denum;

    vec3 lambertDiffuse = kD * matSample.baseColor;

    return (lambertDiffuse / PI + cookTorranceSpecular) * lightRadiance * NdotL;
}

// == SHADER EVALUATION ==

void main()
{
    vec3 normal = normalize(vNormal);
    if (dot(vVertexToCam, normal) < 0) {
        normal = -normal;
    }

    MaterialSample matSample = SampleMaterial();

    vec3 Lo = vec3(0.0);
    for (int i=0 ; i < pointLightCount ; i++) {
        PointLight light = uPointLights[i];

        // Sampling the point light
        vec3 vertexToLight = light.position - vWorldPos;
        float lightDistance = length(vertexToLight);
        vertexToLight /= lightDistance;
        vec3 lightRadiance = light.color / pow(lightDistance + 1.0, light.decay);

        // Computing the BRDF and adding its result to the illumination
        Lo += BRDF_CookTorrance_Microfacet(vertexToLight, vVertexToCam, normal, matSample, lightRadiance);
    }

    Lo *= 1.0 - matSample.transmissionColor;
    Lo += matSample.emissionColor;

    fFragColor = vec4(Lo, 1.0 - (transmissionColor.x + transmissionColor.y + transmissionColor.z) / 3.0);
}
