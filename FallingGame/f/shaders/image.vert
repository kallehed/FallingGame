#version 430 core
layout (location = 0) in vec4 aPos_and_Tex;

out vec2 f_texCoord;

uniform vec2 u_offset;
uniform float u_rotation;

void main()
{
    mat2 rot;
    float v = u_rotation;
    rot[0] = vec2(cos(v),-sin(v));
    rot[1] = vec2(sin(v),cos(v));
    vec2 pos = aPos_and_Tex.xy + u_offset;
    pos += rot * (-1.0*aPos_and_Tex.zw + 0.5);
    gl_Position = vec4(pos.x/1.6, pos.y, 0.0, 1.0);

    f_texCoord = aPos_and_Tex.zw;
}