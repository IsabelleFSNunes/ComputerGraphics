#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 colorPosition;

uniform mat4 gWorld;

out vec3 Color;

void main()
{
    gl_Position = gWorld * vec4(Position, 1.0);
    Color = colorPosition;
}
