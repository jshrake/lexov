#version 150

in vec4 cube_pos;
uniform vec3 chunk_pos;
uniform vec3 normal;
uniform mat4 vp_matrix;
out vec4 frag_color;

void main() {
  if (normal == vec3(0.0, 1.0, 0.0)) {
    frag_color = vec4(1.0, 0.0, 0.0, 1.0);
  } else {
    frag_color = vec4(0.0, 0.0, 1.0, 1.0);
  }
  gl_Position = vp_matrix * vec4(vec3(cube_pos) + chunk_pos, 1);
}
