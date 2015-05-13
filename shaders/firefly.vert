#version 400
layout (location = 0) in vec3 position;

uniform mat4 projectMatrix;
uniform mat4 positionModelMatrix;
uniform mat4 normalModelMatrix;
uniform mat4 modelViewMatrix;
uniform vec3 localLights[100];
uniform int localLightsCount;

out vec3 ourColor;
out vec3 f_position;

void main()
{
	f_position = vec3(positionModelMatrix * vec4(position, 1.0));;

	ourColor = vec3(1, 1, 1);

    gl_Position = projectMatrix * modelViewMatrix * vec4(position, 1.0);
}
