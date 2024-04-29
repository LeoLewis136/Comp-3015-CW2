#version 460

layout (location = 0) in vec3 VertexInitVel;
layout (location = 1) in float VertexBirthTime;

out float Transp;
out vec2 TexCoord;

uniform float Time;

// No gravity
uniform vec3 CameraPos; // Pass in camera current position
uniform float ParticleLifeTime;
uniform float ParticleSize = 1.0f;
uniform vec3 EmitPos;
uniform vec3 BillboardUp;

uniform mat4 ModelIn;
uniform mat4 ViewIn;
uniform mat4 Proj;

const vec3 offsets[] = vec3[](
    vec3(-0.5, -0.5, 0),
    vec3(0.5, -0.5, 0),
    vec3(0.5, 0.5, 0),
    vec3(-0.5, -0.5, 0),
    vec3(0.5, 0.5, 0),
    vec3(-0.5, 0.5, 0));

const vec2 texCoords[] = vec2[] (
    vec2(0, 0),
    vec2(1, 0),
    vec2(1, 1),
    vec2(0,0),
    vec2(1, 1),
    vec2(0, 1)
);

void main(){
    vec3 cameraPos;

    float t = mod(Time - VertexBirthTime, ParticleLifeTime);
    
    vec3 pos = EmitPos + VertexInitVel * t * 0.2;
    vec4 transformedPos = ModelIn * vec4(pos, 1.0f);

    vec3 viewDir = CameraPos - transformedPos.xyz;
    vec3 finalUpVec = normalize(BillboardUp);
    vec3 rightVector = normalize(cross(viewDir, finalUpVec));
    finalUpVec = normalize(cross(rightVector, viewDir));

    // Construct rotation matrix
    mat3 rotationMatrix = mat3(rightVector, finalUpVec, viewDir);

    // Apply rotation to each vertex offset
    vec3 rotatedOffset = rotationMatrix * offsets[gl_VertexID] * ParticleSize;

    cameraPos = vec3(ViewIn * vec4((transformedPos.xyz + rotatedOffset), 1.0f));

    Transp = mix(1, 0, t / ParticleLifeTime);
    

    TexCoord = texCoords[gl_VertexID];
    gl_Position = Proj * vec4(cameraPos , 1.0f);
}