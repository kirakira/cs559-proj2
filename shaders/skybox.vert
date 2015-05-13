#version 400

in vec3 position;

uniform mat4 Projection;
uniform mat4 ModelView;

out vec3 texcoords;

void main () {
  texcoords = position;
  mat4 View = mat4(ModelView);
  View[3][0] = 0.0; View[3][1] = 0.0; View[3][2] = 0.0;
  gl_Position = Projection * View * vec4(position, 1.0);
}