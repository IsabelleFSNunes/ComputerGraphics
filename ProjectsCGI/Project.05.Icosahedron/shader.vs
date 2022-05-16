#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 colorPosition;

uniform mat4 gWorld;

out vec4 Color;

void main()
{
    gl_Position = gWorld * vec4(Position, 1.0);
    Color = vec4(clamp(Position/2.0, 0.0, 1.0), 1.0);
}
