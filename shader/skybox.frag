#version 460

in vec3 ViewPos;
in vec3 Position;

uniform struct FogInfo{
    float MaxDist; // Distance of 100% fog
    float MinDist; // Start distance for fog
    vec3 FogColour; // The colour of the fog
}Fog;

layout (binding = 0) uniform samplerCube skyBoxTex;

layout (location = 0) out vec4 FragColor;

float fogCalculation(){
    float dist = length(ViewPos - Position);
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);
    return clamp(fogFactor, 0.3f, 1.0f);
}

void main(){
    vec3 texColor = texture(skyBoxTex, normalize(Position)).rgb;

    FragColor = vec4(mix(Fog.FogColour, texColor, fogCalculation()), 1.0);
}