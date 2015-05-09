#version 330
layout (location = 0) in vec3 position;

uniform mat4 projectMatrix;
uniform mat4 modelViewMatrix;

out vec3 ourColor;

void main()
{
	float factor = 1 - (position.y + 10) / 50;
	factor = min(factor, 1);
	factor = max(factor, 0);
    ourColor = factor * vec3(.2, .7, .2);
    gl_Position = projectMatrix * modelViewMatrix * vec4(position, 1.0);
}
