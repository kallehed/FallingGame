
layout (location = 0) in vec4 aX_W_Y_H;
layout (location = 1) in vec2 aTex_Z;

out vec2 f_texCoord;
out float f_texture;
out float f_z;

vec2 texPos[4] = vec2[4](
	vec2(0.f, 1.f),
    vec2(0.f, 0.f),
	vec2(1.f, 0.f),
	vec2(1.f, 1.f)
); 

void main()
{
    float pos_x = aX_W_Y_H.x + aX_W_Y_H.y * float(gl_VertexID > 1);
    float pos_y = aX_W_Y_H.z + aX_W_Y_H.w * float(gl_VertexID == 1 || gl_VertexID == 2);

    gl_Position = vec4(pos_x, pos_y, 0.0, 1.0);
    //gl_Position = vec4(gl_InstanceID * 0.1 + gl_VertexID*0.1, gl_VertexID*0.1, 0.0, 1.0);

    f_texCoord = texPos[gl_VertexID];
    f_texture = aTex_Z.x;
    f_z = aTex_Z.y;