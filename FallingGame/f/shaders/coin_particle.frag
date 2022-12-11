#version 430 core

out vec4 FragColor;

in vec2 f_screenCoord;

layout(std140, binding = 0) uniform Globals
{
    float g_death_y;
    float g_cam_y;
    float g_timer;
};

uniform float u_pos_and_vel_and_time[5];

void main()
{
    vec2 pos = vec2(u_pos_and_vel_and_time[0], u_pos_and_vel_and_time[1]); // start pos
    vec2 vel = vec2(u_pos_and_vel_and_time[2], u_pos_and_vel_and_time[3]); // start vel
    float start_time = u_pos_and_vel_and_time[4];

    // move pos based on time
    float time_passed = g_timer -  start_time;

    float y_acc = -1.0;

    pos.y += vel.y * time_passed + 0.5 * y_acc*time_passed*time_passed;
    pos.x += vel.x * time_passed;

    float alpha = 1.0 - 2.0 * distance(f_screenCoord + vec2(0.0, g_cam_y), pos);

    FragColor = vec4(0.0, 0.0, 0.0, alpha);
}