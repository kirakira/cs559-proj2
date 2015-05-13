#version 330
in vec3 ourColor;
in vec3 f_position;
in vec3 f_normal;

uniform vec3 light;
uniform vec3 localLights[100];
uniform int localLightsCount;
  
void main()
{
	gl_FragColor = vec4(ourColor, 1.0f);
}
