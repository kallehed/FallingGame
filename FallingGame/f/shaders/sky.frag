#version 430 core

out vec4 FragColor;

in vec2 f_texCoord;
uniform sampler2D sky_texture;

void main()
{
    FragColor = texture(sky_texture, f_texCoord);
} 