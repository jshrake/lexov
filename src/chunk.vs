#version 410

in vec4 cube_pos;
uniform vec3 chunk_pos;
uniform vec3 normal;
uniform mat4 vp_matrix;
uniform samplerBuffer texture;
out vec4 f_color;
void main() {
  int offset = 4 * int(cube_pos.w);
  if (normal.y == 1 && int(cube_pos.w) == 2) {
    offset = 4;
  } 
  float r = texelFetch(texture, offset + 0).r;
  float g = texelFetch(texture, offset + 1).r;
  float b = texelFetch(texture, offset + 2).r;
  float a = texelFetch(texture, offset + 3).r;
  f_color = vec4(r, g, b, a);
  gl_Position = vp_matrix * vec4(cube_pos.xyz + chunk_pos, 1);
}
