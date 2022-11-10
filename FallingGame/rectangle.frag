#version 330 core
out vec4 FragColor;

in vec2 f_pos;

uniform vec4 u_color;

void main()
{
    FragColor = u_color;
} 