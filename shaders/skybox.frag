#version 400

in vec3 texcoords;
uniform samplerCube tex;

void main () {
  gl_FragColor = texture(tex, texcoords);
  //gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}