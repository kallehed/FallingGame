
layout (location = 0) in vec4 aPos_and_Tex;

out vec2 f_texCoord;

uniform vec2 u_offset;


void main()
{
    vec2 pos = aPos_and_Tex.xy;
    pos = pos + u_offset;
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);

    f_texCoord = aPos_and_Tex.zw;