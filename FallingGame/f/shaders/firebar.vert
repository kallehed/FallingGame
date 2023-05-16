
out vec2 f_texCoord;

uniform vec4 u_offset_width_height;

vec2 poses[4] = vec2[4](
    vec2(-0.5f,-0.5f),
    vec2(-0.5f,0.5f),
    vec2(0.5f,0.5f),
    vec2(0.5f,-0.5f)
);

vec2 texCoords[4] = vec2[4](
    vec2(0.f,0.f),
    vec2(0.f,1.f),
    vec2(1.f,1.f),
    vec2(1.f,0.f)
);

void main()
{
    vec2 pos = u_offset_width_height.xy; 
	pos += poses[gl_VertexID] * u_offset_width_height.zw;

    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);

    f_texCoord = texCoords[gl_VertexID];
