#version 400
in vec3 ourColor;
in vec3 f_position;

uniform vec3 localLights[100];
uniform int localLightsCount;
  
void main()
{
	vec3 combinedLight = vec3(0, 0, 0);

	for (int i = 0; i < localLightsCount; ++i) {
		float dist = distance(localLights[i], f_position);
		float factor = 1.f / max(.01, dist * dist / 30);
		combinedLight += min(vec3(1, 1, 1), factor * vec3(.22, .20, .34));
	}

	gl_FragColor = vec4(min(vec3(1, 1, 1), ourColor * combinedLight), 1.0);
}
