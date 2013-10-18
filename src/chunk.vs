#version 410

in vec4 cube_pos;
uniform vec3 chunk_pos;
uniform mat4 vp_matrix;
uniform mat4 v_matrix;

out vec4 f_world_pos;
out float f_depth;

void main() {
  vec4 pos = vec4(cube_pos.xyz + chunk_pos, 1);
  f_depth = length((v_matrix * pos).xyz);
  f_world_pos = cube_pos;
  gl_Position = vp_matrix * pos;
}
