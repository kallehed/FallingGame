
uniform float u_shiny; 

in vec2 f_texCoord;
uniform sampler2D tex;

#define M_PI (3.1415926535897932384626433832795)

void main()
{
    FragColor = texture(tex, f_texCoord);

	if (0.999  <= u_shiny)
	{
		float px_timer = g_timer*3.0 - 2.5*(1.0 - f_texCoord.x) - 0.5*(1.0 - f_texCoord.y);
		FragColor.r *= 1.3 + 1.0 * sin(px_timer * 1.1 - 0.36);
		FragColor.b += 0.2 + 0.8 * abs(sin(px_timer * 0.9495 - 93.4839));
		FragColor.g *= 1.0 + 0.1 * abs(sin(px_timer*0.8 - 0.483));
	}

    // boost effect when picking up coins
/*    {
        float time = u_time_since_coin * 7.0 - 2.5*(1.0-f_texCoord.y);
        //time *= 2.0;
        float radians = time + M_PI*0.20;
        float change = sin(radians) * 0.66 * float(radians < 2.0* M_PI);
        change = max(0.0,change);
        //change *= float(time < 11.0);
        //change *= float(u_time_since_coin < 2.0);
        FragColor += vec4(change,change,0.0,0.0); 
    }
	*/
