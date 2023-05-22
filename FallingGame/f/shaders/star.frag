
in vec2 f_texCoord;
uniform sampler2D tex;
uniform float u_filled;

#define M_PI (3.1415926535897932384626433832795)

void main()
{
    float filled = 1.0 - u_filled;

    FragColor = texture(tex, f_texCoord);
    FragColor.xyz *= (f_texCoord.y > filled) ? 1.0 : 0.4;
