#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 f_pos;

void main()
{
    f_pos = aPos;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}