#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projectMatrix;
uniform mat4 modelViewMatrix;
uniform float time;
uniform vec3 light;
uniform vec3 localLights[100];
uniform int localLightsCount;

out vec3 ourColor;
out vec3 f_position;
out vec3 f_normal;

void main()
{
	f_position = position;
	f_normal = normal;

	ourColor = vec3(.145, 1, 0);

    gl_Position = projectMatrix * modelViewMatrix * vec4(position, 1.0);
}
