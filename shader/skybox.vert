#version 460

layout (location = 0) in vec3 VertexPosition;

out vec3 ViewPos;
out vec3 Position;

uniform mat4 MVP;
uniform mat4 ModelIn;
uniform vec3 CamPos;

void main(){
    ViewPos = CamPos;
    Position = VertexPosition;
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}