#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 ProjectorMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

out vec3 viewNormal;
out vec4 viewPosition;
out vec4 projTexCoord;

void main() {
	vec4 pos = vec4(position, 1.0);
	viewNormal = normalize(NormalMatrix * normal);
	viewPosition = ModelViewMatrix * pos;
	projTexCoord = ProjectorMatrix * (ModelMatrix * pos);
	gl_Position = MVP * pos;
}
