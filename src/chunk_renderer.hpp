#pragma once
#include "chunk.hpp"
#include "chunk_mesh.hpp"
#include "types.hpp"
#include <mogl/mogl.hpp>
#include <unordered_map>

namespace lexov {
class camera;
class chunk_renderer {
public:
  chunk_renderer(mogl::program program);
  void render(const camera &cam);
  void on_chunk_update(const chunk_key &key, const chunk &c);
  void on_chunk_insertion(const chunk_key &key, const chunk &c);
  void on_chunk_removal(const chunk_key &key);
  void set_program(mogl::program program);

private:
  void update_ogl_ids();
  void build_mesh(chunk_mesh &mesh, const chunk &c);
  void upload_vertex_data(chunk_face_buffer &cfb, const buffer_data &d);
  using chunk_mesh_map =
      std::unordered_map<chunk_key, chunk_mesh, chunk_hash, chunk_hash_equal>;
  chunk_mesh_map meshes;
  mogl::program shader_program{};
  using texture_buffer_object = mogl::buffer<mogl::buffer_type::texture, mogl::buffer_usage::static_draw>;
  texture_buffer_object tbo{};
  mogl::texture<mogl::texture_type::texture_buffer> tbo_texture{};

  GLuint cube_pos_attrib_id;
  GLuint chunk_pos_uniform_id;
  GLuint normal_uniform_id;
  GLuint camera_pos_uniform_id;
  GLuint texture_uniform_id; 

  static const std::string cube_pos_attrib_name;
  static const std::string chunk_pos_uniform_name;
  static const std::string normal_uniform_name;
  static const std::string camera_pos_uniform_name;
  static const std::string texture_uniform_name;
  
};


} // namespace lexov
