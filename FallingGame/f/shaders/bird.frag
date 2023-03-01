#version 430 core

out vec4 FragColor;

uniform float u_time_since_coin; 

in vec2 f_texCoord;
in vec2 f_screenCoord;
uniform sampler2D bird_texture;

layout(std140, binding = 0) uniform Globals
{
    float g_death_y;
    float g_cam_y;
    float g_timer;
    float g_w;
};

#define M_PI (3.1415926535897932384626433832795)

void main()
{
    FragColor = texture(bird_texture, f_texCoord);

    // boost effect when picking up coins
    {
        float time = u_time_since_coin * 5 - 2.5*(1-f_texCoord.y);
        //time *= 2;
        float radians = time + M_PI*0.20;
        float change = sin(radians) * 0.66 * float(radians < 2* M_PI);
        change = max(0,change);
        //change *= float(time < 11.0);
        //change *= float(u_time_since_coin < 2.0);
        FragColor += vec4(change,change,0.f,0.f); 
    }
}