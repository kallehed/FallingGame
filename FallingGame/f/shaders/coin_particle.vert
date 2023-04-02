
layout (location = 0) in vec2 aPos;

void main()
{
    vec2 pos = aPos.xy;
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);