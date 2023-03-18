
layout (location = 0) in vec2 aChar_XOffset;

uniform vec3 u_offset_scale;

uniform sampler2D u_font_tex;

uniform vec4 u_text[128]; // contains character width, height, bearingX, bearingY
uniform float u_char_x_offsets[128]; // contains the x offset in the big font texture for each character

out vec2 f_texCoord;

void main()
{
    vec2 u_offset = u_offset_scale.xy;
    float u_scale = u_offset_scale.z;
    // character
    int c = int(aChar_XOffset.x);
    float x_offset = aChar_XOffset.y;

    float width = u_text[c].x * float(gl_VertexID > 1);
    float height = u_text[c].y * float(gl_VertexID == 1 || gl_VertexID == 2);
    // position

    float pos_x = x_offset + u_offset.x + u_scale * (u_text[c].z + width);
    float pos_y =             u_offset.y + u_scale * (u_text[c].w - height);

    vec2 pos = vec2(pos_x, pos_y);
    gl_Position = vec4(pos.x/g_w, pos.y, 0.0, 1.0);

    f_texCoord = vec2((u_char_x_offsets[c] + width), height);
    ivec2 asd = textureSize(u_font_tex, 0);
    f_texCoord /= vec2(asd.x, asd.y);
    //f_texCoord /= vec2(2099.0, 45.0);
    
}