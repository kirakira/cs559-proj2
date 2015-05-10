#version 400

in vec3 position;

uniform mat4 Projection;
uniform mat4 ModelView;

out vec3 texcoords;

void main () {
  texcoords = position;
  gl_Position = Projection * ModelView * vec4(position, 1.0);
}