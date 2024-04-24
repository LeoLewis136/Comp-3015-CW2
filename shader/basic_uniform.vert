#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

out vec2 TexCoord;
out vec3 Position;
out vec3 Normal;

out vec3 ViewPos;


// Projection
uniform mat4 ModelIn;
uniform mat4 MVP;
uniform vec3 CamPos;

void main()
{
    ViewPos = CamPos;

    TexCoord = VertexTexCoord;
    Position = VertexPosition;
    Normal = VertexNormal;

    gl_Position = MVP * vec4(VertexPosition.x, VertexPosition.y, VertexPosition.z, 1.0f);
}
