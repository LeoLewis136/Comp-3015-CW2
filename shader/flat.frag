#version 460

uniform vec4 Colour;
layout (location = 0) out vec4 FragColor;

void main(){
    FragColor = Colour;
}