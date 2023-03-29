

out vec4 FragColor;

uniform float u_time_since_coin; 

in vec2 f_texCoord;
in vec2 f_screenCoord;
uniform sampler2D bird_texture;


#define M_PI (3.1415926535897932384626433832795)

void main()
{
    FragColor = texture(bird_texture, f_texCoord);

    // boost effect when picking up coins
    {
        float time = u_time_since_coin * 7.0 - 2.5*(1.0-f_texCoord.y);
        //time *= 2.0;
        float radians = time + M_PI*0.20;
        float change = sin(radians) * 0.66 * float(radians < 2.0* M_PI);
        change = max(0.0,change);
        //change *= float(time < 11.0);
        //change *= float(u_time_since_coin < 2.0);
        FragColor += vec4(change,change,0.0,0.0); 
    }
}
