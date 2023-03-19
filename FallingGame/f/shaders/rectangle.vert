
layout (location = 0) in vec2 aPos;



void main()
{
    //vec2 pos = aPos + u_offset;
    vec2 pos = aPos + vec2(0.0, -g_cam_y);
    gl_Position = vec4(pos.x/g_w, pos.y, 0.0, 1.0);
}