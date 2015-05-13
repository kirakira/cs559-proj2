#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

out vec2 texPos;

void main() {
	
	mat4 ModelView = mat4(ModelViewMatrix);

	ModelView[0][0] = 1.0; ModelView[0][1] = 0.0; ModelView[0][2] = 0.0;
	ModelView[1][0] = 0.0; ModelView[1][1] = 1.0; ModelView[1][2] = 0.0;
	ModelView[2][0] = 0.0; ModelView[2][1] = 0.0; ModelView[2][2] = 1.0;
	
	texPos= texCoord;
	gl_Position = ProjectionMatrix * ModelView * vec4(position, 1.0);
}
