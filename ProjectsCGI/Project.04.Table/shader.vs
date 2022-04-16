#version 330

layout (location = 0) in vec3 Position;

uniform mat4 gWorld;

out vec4 Color;

void main()
{
    gl_Position = gWorld * vec4(Position, 1.0);
    Color = vec4( 171.0/255.0, 127.0/255.0, 101.0/255.0, 1.0);
}
