
out vec4 FragColor;

in vec2 f_texCoord;

uniform sampler2D u_font_tex;

uniform vec4 u_color;

void main()
{
    //vec4 textColor = vec4(1.0,1.0,1.0, 1.0 - texture(u_font_tex, f_texCoord * 0.1).r);
    vec4 textColor = vec4(1.0, 1.0, 1.0, texture(u_font_tex,f_texCoord).r);
    FragColor = u_color * textColor;
    //FragColor.r = 255;
}