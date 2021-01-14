#version 330

layout(location = 0) in vec2 pos;
layout(location = 1) in float col;

uniform mat4 mat;

out float vertexColor;

void main()
{
	gl_Position = mat * vec4(pos, 0.0, 1.0);
	vertexColor = col;
}
