
layout (location = 0) in vec4 aPos_and_Tex;

out vec2 f_texCoord;

void main()
{
    vec2 pos = aPos_and_Tex.xy;
    float y_off = g_cam_y;
    float img_h = 0.8;
    y_off -= img_h * float(int((y_off)/img_h));
    pos = pos - vec2(0.f, y_off);
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);

    f_texCoord = aPos_and_Tex.zw;