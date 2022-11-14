#version 430 core

out vec4 FragColor;

in vec2 f_texCoord;
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, f_texCoord);
} 