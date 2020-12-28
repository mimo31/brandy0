#version 330

layout(location = 0) in vec4 pos;
layout(location = 1) in float col;

uniform mat4 mat;

out float vertexColor;

void main()
{
	gl_Position = mat * pos;
	vertexColor = col;
}
