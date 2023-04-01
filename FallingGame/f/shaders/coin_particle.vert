
layout (location = 0) in vec2 aPos;

out vec2 f_screenCoord;

void main()
{
    vec2 pos = aPos.xy;
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);

    f_screenCoord = gl_Position.xy;