#version 460 core

// == INPUTS ==

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoords;
in float vDepth;


// == UNIFORM BUFFERS ==

layout(std140, binding = 0) uniform MaterialInputs
{
    vec3  surfaceColor;
    bool  surfaceColorUseTexture;

    vec3  deepColor;
    bool  deepColorUseTexture;
};


// == UNIFORMS ==

uniform sampler2D uTextures[2];
uniform float uTime = 0.0;

struct PointLight
{
    vec3 position;
    vec3 color;
    float decay;
};

const int pointLightCount = 1;
uniform PointLight uPointLights[pointLightCount];

// == OUTPUTS ==

out vec4 fFragColor;


// == CONSTANTS ==

const int surfaceColorTexture = 0;
const int deepColorTexture = 1;


// == HELPER FUNCTIONS ==

vec2 RandomVec2(vec2 uv){
    uv = vec2(dot(uv, vec2(151.41, 351.753)),
              dot(uv, vec2(684.535,427.324)));

    return fract(sin(uv)*752196.65962) * 2.0 - 1.0;
}

// From Ignio Quilez perlin noise implementation
float PerlinNoise(vec2 uv) {
    vec2 i = floor(uv);
    vec2 f = fract(uv);

    vec2 g = f * f * (3.0 - 2.0 * f);

    return mix( mix( dot( RandomVec2(i + vec2(0.0, 0.0) ), f - vec2(0.0, 0.0) ),
                     dot( RandomVec2(i + vec2(1.0, 0.0) ), f - vec2(1.0, 0.0) ), g.x),
                mix( dot( RandomVec2(i + vec2(0.0, 1.0) ), f - vec2(0.0, 1.0) ),
                     dot( RandomVec2(i + vec2(1.0, 1.0) ), f - vec2(1.0, 1.0) ), g.x), g.y);
}

float TurbulentPerlinNoise(vec2 pos, float freq, float offset)
{
    float ns = PerlinNoise(pos * freq + offset);
    ns += PerlinNoise(pos * freq + vec2(2.745, 8.6355) + vec2(offset, -offset));
    ns += PerlinNoise(pos * freq + vec2(2578.7785245, 42.124366355) + vec2(-offset, 0.0));

    return ns;
}


// == MATERIAL SAMPLING FUNCTIONS ==

float LightAttenuation(float distance, float lightDecay)
{
    return 1.0 / pow(distance, lightDecay);
}

vec3 SampleSurfaceColor(vec2 uv)
{
    return mix(surfaceColor, 
               texture(uTextures[surfaceColorTexture], uv).rgb,
               float(surfaceColorUseTexture));
}

vec3 SampleDeepColor(vec2 uv)
{
    return mix(deepColor, 
               texture(uTextures[deepColorTexture], uv).rgb, 
               float(deepColorUseTexture));
}

vec3 SampleMaterial(vec3 pos, vec3 normal)
{
    // using world pos as uv to allow seamless transitions between adjacent water cells
    vec2 uvAtPos = pos.xz * 1.0;

    // Sample material inputs
    vec3 surface = SampleSurfaceColor(fract(uvAtPos));
    vec3 deep = SampleDeepColor(fract(uvAtPos));

    // Compute wavelet uv distortion
    vec2 wavelets = vec2(TurbulentPerlinNoise(uvAtPos, 15.0, uTime * 0.1 + 15723.01485), 
                         TurbulentPerlinNoise(uvAtPos, 15.0, -uTime * 0.1 + 15723.01485)) * 0.5 + 0.5;
    vec2 distortedUv = uvAtPos + wavelets * 0.3;

    // Compute light power
    vec3 Lo = vec3(0.0);
    for (int i=0 ; i < pointLightCount ; i++) {
        PointLight light = uPointLights[i];

        vec3 lightDir = light.position - pos;
        float lightDistance = length(lightDir);
        lightDir /= lightDistance;
        lightDistance += 1.0; // Using (distance - 1.0) to compute the attenuation over the distance to avoid having a singularity when distance < 1.0 or clamping 
        float lightAttenuation = LightAttenuation(lightDistance, light.decay);
        vec3 incomingLight = light.color * lightAttenuation;

        // Compute diffuse brdf
        float absCosTheta = max(dot(lightDir, normal), 0.0);
        float wavesFlow = PerlinNoise(uvAtPos + uTime * 0.1) + PerlinNoise(uvAtPos - uTime * 0.1) * 0.5 + 0.5;
        
        float waves = wavesFlow;
        waves *= (0.66 + 0.33 * TurbulentPerlinNoise(uvAtPos, 2.0, uTime * 0.5));
        vec3 wavesColor = mix(surface, deep, waves) * incomingLight * absCosTheta;

        float specularWaves = TurbulentPerlinNoise(distortedUv * 8.0, 1.0, uTime * 0.1) * 0.5 + 0.5;
        float specularAttenuation = LightAttenuation(lightDistance, 0.8);  // The "specular" of the water decays slower than the ligth diffues
        float specular = float(specularWaves * (waves * 0.6 + 0.4) * specularAttenuation * 1.6> 0.35);
        vec3 specularColor = vec3(specular) * waves * incomingLight*incomingLight * 0.2 ;

        Lo += wavesColor + specularColor;
    }

    return Lo;
}

// == SHADER EVALUATION ==

void main() 
{
    vec3 color = SampleMaterial(vWorldPos, normalize(vNormal));

    fFragColor = vec4(color, 1.0);
}
