

uniform vec2 u_time_since_coin_and_powerup; 

in vec2 f_texCoord;
uniform sampler2D bird_texture;


#define M_PI (3.1415926535897932384626433832795)

void main()
{
	float time_since_coin = u_time_since_coin_and_powerup.x;
	float powerup = u_time_since_coin_and_powerup.y;


    FragColor = texture(bird_texture, f_texCoord);

    // boost effect when picking up coins
    {
        float time = time_since_coin * 7.0 - 2.5*(1.0-f_texCoord.y);
        //time *= 2.0;
        float radians = time + M_PI*0.20;
        float change = sin(radians) * 0.66 * float(radians < 2.0* M_PI);
        change = max(0.0,change);
        //change *= float(time < 11.0);
        //change *= float(time_since_coin < 2.0);
        FragColor += vec4(change,change,0.0,0.0); 
    }

	// powerup effect when having powerup 
	if (powerup >= 0.99)
	{	
		FragColor *= 1.2;
	}
