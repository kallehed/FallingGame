

out vec2 f_texCoord;
out vec2 f_screenCoord;

uniform vec2 u_offset;
uniform vec3 u_rotation_width_height;

vec2 poses[4] = vec2[4](
    vec2(-0.5f,-0.5f),
    vec2(-0.5f,0.5f),
    vec2(0.5f,0.5f),
    vec2(0.5f,-0.5f)
);

vec2 texCoords[4] = vec2[4](
    vec2(1.f,1.f),
    vec2(1.f,0.f),
    vec2(0.f,0.f),
    vec2(0.f,1.f)
);

void main()
{
    float width = u_rotation_width_height.y;
    float height = u_rotation_width_height.z;
    float v = u_rotation_width_height.x;

    mat2 rot;
    rot[0] = vec2(cos(v),-sin(v));
    rot[1] = vec2(sin(v),cos(v));

    vec2 pos = poses[gl_VertexID] * vec2(width, height);
    pos = rot * pos;
    pos = pos + u_offset;
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);

    f_texCoord = texCoords[gl_VertexID];
    f_screenCoord = gl_Position.xy;
