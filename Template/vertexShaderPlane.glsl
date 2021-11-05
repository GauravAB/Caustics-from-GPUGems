#version 460 core

in vec4 vPosition;
uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;
out vec3 fPos;


void main(void)
{
	vec3 pos = vPosition.xyz;

	fPos = pos.xyz;

	gl_Position = u_projection_matrix  * u_view_matrix * u_model_matrix * vPosition;
}







