#version 430 core
layout (location = 0) in vec2 aPos;

uniform vec2 u_offset;

void main()
{
    vec2 pos = aPos + u_offset;
    gl_Position = vec4(pos.x/1.6, pos.y, 0.0, 1.0);
}