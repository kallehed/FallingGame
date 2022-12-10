#version 430 core

out vec4 FragColor;

in vec2 f_texCoord;
uniform sampler2D texture1;

layout(std140, binding = 0) uniform Globals
{
    float g_death_y;
    float g_cam_y;
};

void main()
{
    FragColor = texture(texture1, f_texCoord * 1.f);
    float change = min((g_death_y - g_cam_y + 2.0) * 0.2, 1.0);
    FragColor *= vec4(vec3(change), 1.0);
}