#version 330

layout(location = 0) in vec4 pos;
uniform mat4 mat;

void main()
{
	gl_Position = mat * pos;
}
