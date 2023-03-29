

out vec4 FragColor;

in vec2 f_screenCoord;

uniform float u_pos_and_vel_and_time[5];

void main()
{
    vec2 start_pos = vec2(u_pos_and_vel_and_time[0], u_pos_and_vel_and_time[1]); // start pos
    vec2 vel = vec2(u_pos_and_vel_and_time[2], u_pos_and_vel_and_time[3]); // start vel
    float start_time = u_pos_and_vel_and_time[4];

    // move pos based on time
    float time_passed = g_timer -  start_time;
    float y_acc = 0.8, y_jerk = 0.1, x_acc = -1.3;

    vec2 screen_pos = vec2(f_screenCoord.x * g_w, f_screenCoord.y + g_cam_y);
    
    float alpha = 0.0;

    for (int i = 0; i < 5; ++i) {
        vec2 new_pos = start_pos;
        float t1 = time_passed, t2 = t1*t1 / 2.0, t3 = t2*t1 / 3.0; 
        new_pos.y += vel.y*t1 + y_acc*t2 + y_jerk*t3;
        new_pos.x += vel.x*t1 + x_acc*t2;

        float decr_radi = ( /*i * 0.0 + */ 1.0);

        //float change = max(0.0, 1.0 - decr_radi* 100.0 * pow(distance(screen_pos, new_pos), (1.5)));
        float change = max(0.0, 1.0 - decr_radi* 100.0 * pow(abs(screen_pos.x - new_pos.x) + abs(screen_pos.y-new_pos.y), (1.5)));
        change /= float(i) * 0.5 + 1.0; // decrease intensity based on time
        change *= float(time_passed >= 0.0); // do not display rays before it existed
        alpha += change;

        time_passed -= 0.015 * (1.0 / (1.0 + 0.05*float(i)));
    }


    FragColor = vec4(alpha * 2.0, 1.0, 0.0, alpha);
    //FragColor = vec4(255.0/255.0, 204.0/255.0, 106.0/255.0, FragColor.a);
}
