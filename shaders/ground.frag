#version 400
in vec3 ourColor;
in vec3 f_position;
in vec3 f_normal;

uniform vec3 light;
uniform int enableLight;
uniform vec3 localLights[100];
uniform int localLightsCount;
  
void main()
{
	vec3 combinedLight = vec3(0, 0, 0);

	if (enableLight != 0) {
		vec3 v1 = light - f_position;
		float factor = dot(v1, f_normal);
		if (factor > .001) {
			factor /= length(v1) * length(f_normal);
		} else
			factor = 0;
		combinedLight += factor * vec3(1, 1, 1);
	}

	for (int i = 0; i < localLightsCount; ++i) {
		vec3 v1 = localLights[i] - f_position;
		float factor = dot(v1, f_normal);
		float dist = distance(localLights[i], f_position);
		if (factor > .001) {
			factor = factor / length(v1) / length(f_normal) / max(.01, dist * dist / 10);
		} else
			factor = 0;
		combinedLight += min(vec3(1, 1, 1), factor * vec3(.22, .20, .34));
	}

	gl_FragColor = vec4(min(vec3(1, 1, 1), ourColor * combinedLight), 1.0);
}
