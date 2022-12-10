#version 430 core

out vec4 FragColor;

in vec2 f_texCoord;
uniform sampler2D sky_texture;

layout(std140, binding = 0) uniform Globals
{
    float g_death_y;
    float g_cam_y;
};

void main()
{
    FragColor = texture(sky_texture, f_texCoord);
    float change = min((g_death_y - g_cam_y + 2.0) * 0.25, 1.0);
    FragColor *= vec4(vec3(change), 1.0);
}