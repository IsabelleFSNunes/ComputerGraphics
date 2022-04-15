#version 330

in vec4 Color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(fragColor.r, fragColor.r, fragColor.r, 1);
}