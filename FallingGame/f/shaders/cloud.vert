#version 430 core
layout (location = 0) in vec4 aPos_and_Tex;

out vec2 f_texCoord;
out vec2 f_screenCoord;

void main()
{
    vec2 pos = aPos_and_Tex.xy;
    gl_Position = vec4(pos.x/1.6, pos.y, 0.0, 1.0);

    f_texCoord = aPos_and_Tex.zw;
    f_screenCoord = gl_Position.xy;
}