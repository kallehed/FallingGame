

in vec2 f_texCoord;
uniform sampler2D sky_texture;


void main()
{
    FragColor = texture(sky_texture, f_texCoord);
    float change = min((g_death_y - g_cam_y + 2.0) * 0.25, 1.0);
    FragColor *= vec4(vec3(change), 1.0);