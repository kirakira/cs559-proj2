#version 400

in vec3 viewNormal;
in vec4 viewPosition;
in vec4 projTexCoord;

uniform sampler2D ProjectorTex;

vec3 phongModel ( vec3 pos, vec3 normal ) {
	vec3 Ka = vec3( 0.3, 0.3, 0.3 );
	vec3 Ks = vec3( 1.0, 1.0, 1.0 );
	vec3 Kd = vec3( 0.5, 0.5, 0.5 );
	float Shining = 0.5;
	vec3 Intensity = vec3( 0.8, 0.8, 0.8 );
	vec4 LightPosition = vec4( 100.0, 100.0, 100.0, 1.0 );

	vec3 s = normalize(vec3(LightPosition) - pos);
	vec3 v = normalize(-pos.xyz);
	vec3 r = reflect( -s, normal );
	
	vec3 ambient = Intensity * Ka;

	float s_n = max ( dot( s , normal ), 0.0 );
	vec3 diffuse = Intensity * Kd * s_n;
	
	vec3 spec = vec3(0.0);
	if (s_n > 0.0 )
		spec = Intensity * Ks * pow( max ( dot(r,v) , 0.0 ) , Shining );
	return ambient + diffuse + spec;
}

void main() {
	vec3 color = phongModel(vec3(viewPosition), viewNormal);
	vec4 projTexColor = vec4(0.0);
	if (projTexCoord.z > 0.0) {
		projTexColor = textureProj(ProjectorTex, projTexCoord);
	}
	gl_FragColor = vec4(color, 1.0) + projTexColor * 0.5;
}