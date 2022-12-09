#version 430 core

out vec4 FragColor;

in vec2 f_texCoord;
in vec2 f_screenCoord;
uniform sampler2D sky_texture;

uniform float u_death_y;

void main()
{
    FragColor = texture(sky_texture, f_texCoord);

    // make it blurier towards edges
    FragColor *= (1.0 - (abs(0.3*f_screenCoord.y) + abs(0.3*f_screenCoord.x)));

    {
        float dif = (u_death_y + 2.0) - f_screenCoord.y; // difference to colors, but not alpha
        dif *= 0.2f;

        dif = clamp(dif, 0.25, 1.6); // limit value, nnot too black or too bright

        FragColor *= vec4(vec3(dif), 1.f);
    }
}