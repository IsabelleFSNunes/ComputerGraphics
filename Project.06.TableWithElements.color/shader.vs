#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 normal;


uniform mat4 gWVP;

out vec3 Color;

void main()
{
    gl_Position = gWVP * vec4(Position, 1.0);
    Color = inColor;
}
