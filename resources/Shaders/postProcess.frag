#version 460 core

// == INPUTS ==

in vec2 vTexCoords;

// == UNIFORMS ==

uniform sampler2D uBeauty;
uniform sampler2D uDepth;

// == OUTPUTS ==

out vec4 fFragColor;


// == HELPER FUNCTIONS ==

vec3 AddExponentialFog(vec3 baseColor, float distance, float density) 
{
    float fogAmount = 1.0 - exp( -distance * density );
    vec3 fogColor = vec3(0.5, 0.6, 0.7);
    return fogColor * fogAmount + baseColor * (1.0 - fogAmount);
}

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


// == SHADER EVALUATION ==

void main()
{
    vec3 color = texture(uBeauty, vTexCoords).rgb;

    color = Tonemap_ACESFilmic(color);
    color = ODT_Gamma(color, 2.2);  // Pseudo sRGB ODT;

    fFragColor = vec4(color, 1.0);

    // fFragColor = vec4(vTexCoords, 1.0, 1.0);
}
