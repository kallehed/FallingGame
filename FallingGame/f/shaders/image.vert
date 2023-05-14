
layout (location = 0) in vec4 aPos_and_Tex;

out vec2 f_texCoord;

uniform vec3 u_offset_and_to_cam;
uniform float u_rotation;


void main()
{
    float v = u_rotation;
    mat2 rot;
    rot[0] = vec2(cos(v),-sin(v));
    rot[1] = vec2(sin(v),cos(v));

    vec2 pos = aPos_and_Tex.xy;
    pos = rot * pos;
    pos = pos + u_offset_and_to_cam.xy + vec2(0.0, -g_cam_y) * u_offset_and_to_cam.z;

    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);

    f_texCoord = aPos_and_Tex.zw;