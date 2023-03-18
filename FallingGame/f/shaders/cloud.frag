

out vec4 FragColor;

in vec2 f_texCoord;
in vec2 f_screenCoord;
in float f_texture;
in float f_z;

uniform sampler2D cloudTexs[4];

void main()
{
    int tex_idx = int(f_texture);
    FragColor = texture(cloudTexs[tex_idx], f_texCoord);
    
    // blur effect
    
    {
        float c = 0.01 + 0.075*(f_z - 0.25);
        FragColor += texture(cloudTexs[tex_idx], f_texCoord + vec2(c, 0.0));
        FragColor += texture(cloudTexs[tex_idx], f_texCoord + vec2(-c, 0.0));
        FragColor += texture(cloudTexs[tex_idx], f_texCoord + vec2(0.0, c));
        FragColor += texture(cloudTexs[tex_idx], f_texCoord + vec2(0.0, -c));
        FragColor *= 1.0/5.0;
        FragColor *= float(FragColor.a > 0.5);
    }
    

    {
        // make dimer when closer to death
        float change = min((g_death_y - g_cam_y + 2.0) * 0.2, 1.0);
        FragColor *= vec4(vec3(change), 1.0);
    }

    // bloom effect
    {
        float change = 1.05 -  abs(0.1*f_screenCoord.x + 0.2*f_screenCoord.y);
        FragColor *= vec4(vec3(change), max(-111.9, change));
    }
    
}