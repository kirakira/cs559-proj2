#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projectMatrix;
uniform mat4 positionModelMatrix;
uniform mat4 normalModelMatrix;
uniform mat4 modelViewMatrix;
uniform float time;
uniform vec3 light;
uniform vec3 localLights[100];
uniform int localLightsCount;
uniform vec3 color;

out vec3 ourColor;
out vec3 f_position;
out vec3 f_normal;

void main()
{
	f_position = vec3(positionModelMatrix * vec4(position, 1.0));
	f_normal = vec3(normalModelMatrix * vec4(normal, 1.0));

	ourColor = color;

    gl_Position = projectMatrix * modelViewMatrix * vec4(position, 1.0);
}
