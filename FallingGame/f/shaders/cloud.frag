#version 430 core

out vec4 FragColor;

in vec2 f_texCoord;
in vec2 f_screenCoord;
uniform sampler2D cloud_texture;

layout(std140, binding = 0) uniform Globals
{
    float g_death_y;
    float g_cam_y;
};

uniform float u_z;

void main()
{
    FragColor = texture(cloud_texture, f_texCoord);
    
    // blur effect
    {
        float c = 0.01 + 0.075*(u_z - 0.3);
        FragColor += texture(cloud_texture, f_texCoord + vec2(c, 0.0));
        FragColor += texture(cloud_texture, f_texCoord + vec2(-c, 0.0));
        FragColor += texture(cloud_texture, f_texCoord + vec2(c, c));
        FragColor += texture(cloud_texture, f_texCoord + vec2(-c, c));
        FragColor += texture(cloud_texture, f_texCoord + vec2(c, -c));
        FragColor += texture(cloud_texture, f_texCoord + vec2(-c, -c));
        FragColor += texture(cloud_texture, f_texCoord + vec2(0.0, c));
        FragColor += texture(cloud_texture, f_texCoord + vec2(0.0, -c));
        FragColor *= 1.0/9.0;
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