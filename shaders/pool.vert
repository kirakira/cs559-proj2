#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projectMatrix;
uniform mat4 modelViewMatrix;
uniform float time;

out vec3 ourColor;

#define M_PI 3.1415926535897932384626433832795

void main()
{
	float y = .07 * sin((.3 * position.x + .7 * position.y) / 2 + time * M_PI) * position.x;
    ourColor = vec3(.3, .58, 1);
    gl_Position = projectMatrix * modelViewMatrix * vec4(position.x, y, position.z, 1.0);
}
