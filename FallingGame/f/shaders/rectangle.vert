#version 430 core
layout (location = 0) in vec2 aPos;

uniform vec2 u_offset;

layout(std140, binding = 0) uniform Globals
{
    float g_death_y;
    float g_cam_y;
    float g_timer;
    float g_w;
};

void main()
{
    vec2 pos = aPos + u_offset;
    gl_Position = vec4(pos.x/g_w, pos.y, 0.0, 1.0);
}