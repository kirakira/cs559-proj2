#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projectMatrix;
uniform mat4 modelViewMatrix;
uniform float time;
uniform vec3 light;
uniform vec3 localLights[100];
uniform int localLightCount;

out vec3 ourColor;
out vec3 f_position;
out vec3 f_normal;

#define M_PI 3.1415926535897932384626433832795

void main()
{
	float y = .07 * sin((.3 * position.x + .7 * position.y) / 2 + time * M_PI) * position.x;
	vec3 final_position = vec3(position.x, y, position.z);

	f_position = final_position;
	f_normal = normal;
	if (sin(position.x) > 0)
		ourColor = vec3(.2, .2, .2);
	else
		ourColor = vec3(.4, .2, 1);

    gl_Position = projectMatrix * modelViewMatrix * vec4(final_position, 1.0);
}
