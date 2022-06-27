#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 inColor;

uniform mat4 gWVP;
uniform mat4 gProjection;
uniform vec4 ClipPlane;

flat out vec3 Color;

out float gl_ClipDistance[1];

void main()
{
    gl_Position = gProjection  * gWVP * vec4(Position, 1.0);
    Color = inColor;
    gl_ClipDistance[0] = dot(gWVP * vec4(Position, 1.0), ClipPlane);
}
