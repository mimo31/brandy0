#version 330

layout(location = 0) in vec2 pos;
uniform mat4 mat;

void main()
{
	gl_Position = mat * vec4(pos, 0.0, 1.0);
}
