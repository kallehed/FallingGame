
out vec4 FragColor;

in vec2 f_texCoord;
in vec2 f_screenCoord;
uniform sampler2D sky_texture;

void main()
{
    FragColor = texture(sky_texture, f_texCoord);

    // make it blurier towards edges
    //FragColor *= (1.0 - (abs(0.2*f_screenCoord.y) + abs(0.2*f_screenCoord.x)));
    FragColor *= vec4(1.0 - (abs(0.4*f_screenCoord.y + 0.6*f_screenCoord.x)));
    FragColor.a = max(FragColor.a, 0.75);
    {
        float dif = (g_death_y - g_cam_y + 2.0) - f_screenCoord.y; // difference to colors, but not alpha
        dif *= 0.2f;

        dif = clamp(dif, 0.25, 1.4); // limit value, nnot too black or too bright

        FragColor *= vec4(vec3(dif), 1.f);
    }
}