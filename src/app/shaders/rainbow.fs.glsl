#version 330

in float vertexColor;

out vec4 outputColor;

void main()
{
	if (vertexColor < .2)
	{
		float t = vertexColor * 5;
		outputColor = vec4(0.0, 0.0, t, 1.0);
	}
	else if (vertexColor < .4)
	{
		float t = (vertexColor - .2) * 5;
		outputColor = vec4(0.0, t, 1.0, 1.0);
	}
	else if (vertexColor < .6)
	{
		float t = (vertexColor - .4) * 5;
		outputColor = vec4(0.0, 1.0, 1.0 - t, 1.0);
	}
	else if (vertexColor < .8)
	{
		float t = (vertexColor - .6) * 5;
		outputColor = vec4(t, 1.0, 0.0, 1.0);
	}
	else
	{
		float t = (vertexColor - .8) * 5;
		outputColor = vec4(1.0, 1.0 - t, 0.0, 1.0);
	}
}
