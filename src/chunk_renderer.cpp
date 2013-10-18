#include "chunk_renderer.hpp"
#include "camera.hpp"
#include <iostream>
namespace lexov {

const std::string chunk_renderer::cube_pos_attrib_name = "cube_pos";
const std::string chunk_renderer::chunk_pos_uniform_name = "chunk_pos";
const std::string chunk_renderer::normal_uniform_name = "normal";
const std::string chunk_renderer::camera_pos_uniform_name = "vp_matrix";
const std::string chunk_renderer::texture_uniform_name = "texture";
const std::string chunk_renderer::view_matrix_name = "v_matrix";

chunk_renderer::chunk_renderer(mogl::program program)
    : shader_program{ std::move(program) } {
  update_ogl_ids();
}

void chunk_renderer::set_program(mogl::program program) {
  shader_program = std::move(program);
  update_ogl_ids();
}

void chunk_renderer::update_ogl_ids() {
  cube_pos_attrib_id =
      shader_program.get_attribute_location(cube_pos_attrib_name);
  chunk_pos_uniform_id =
      shader_program.get_uniform_location(chunk_pos_uniform_name);
  normal_uniform_id = shader_program.get_uniform_location(normal_uniform_name);
  camera_pos_uniform_id =
      shader_program.get_uniform_location(camera_pos_uniform_name);
  texture_uniform_id =
      shader_program.get_uniform_location(texture_uniform_name);
  view_matrix_uniform_id =
      shader_program.get_uniform_location(view_matrix_name);

  float colors[] = { 0.0f, 0.0f, 0.0f, 0.0f,                      // nothing
                     0.0f, 204.0f / 255.0f, 51.0f / 255.0f, 1.0f, // grass
                     87 / 255.0f, 59 / 255.0f, 12 / 255.0f, 1.0f, // dirty
                     0.0f, 0.0f, 1.0f, 1.0f,                      // water
                     50 / 255.0f, 50 / 255.0f, 50 / 255.0f, 1.0f, // stone
  };
  tbo.data(colors);
  mogl::active_texture(0);
  tbo_texture.bind();
  tbo.tex_buffer(GL_R32F);
}

void chunk_renderer::render(const camera &cam) {
  auto shader_program_scope = shader_program.scope_bind();
  // Make sure our texture buffer object is active
  mogl::active_texture(0);
  tbo_texture.bind();
  CHECKED_CALL(glUniform1i(texture_uniform_id, 0));
  // Set the camera matrix
  CHECKED_CALL(glUniformMatrix4fv(camera_pos_uniform_id, 1, GL_FALSE,
                                  cam.get_view_projection()));
  CHECKED_CALL(glUniformMatrix4fv(view_matrix_uniform_id, 1, GL_FALSE,
                                  cam.get_view_matrix()));
  for (const auto &itr : meshes) {
    const auto &pos = itr.first;
    const auto &mesh = itr.second;
    // Set the chunk position
    const auto chunk_x = std::get<0>(pos);
    const auto chunk_y = std::get<1>(pos);
    const auto chunk_z = std::get<2>(pos);
    const auto world_chunk_x = chunk_x * chunk_width;
    const auto world_chunk_y = chunk_y * chunk_height;
    const auto world_chunk_z = chunk_z * chunk_depth;
    const auto world_center_chunk_x = world_chunk_x + half_chunk_width;
    const auto world_center_chunk_y = world_chunk_y + half_chunk_height;
    const auto world_center_chunk_z = world_chunk_z + half_chunk_depth;
    const auto chunk_clip_position =
        cam.world_to_clip(std::array<decltype(world_center_chunk_x), 3>{
          { world_center_chunk_x, world_center_chunk_y, world_center_chunk_z }
        });
    const auto chunk_diameter_clip =
        (float)chunk_diameter / std::abs(chunk_clip_position[3]);
    // if the chunk is outside of the frustrum, don't draw it
    if ((chunk_clip_position[2] < -chunk_diameter) ||
        (std::fabs(chunk_clip_position[0]) > 3 + chunk_diameter_clip ||
         std::fabs(chunk_clip_position[1]) > 3 + chunk_diameter_clip)) {
      continue;
    }

    // Update the world_chunk position
    CHECKED_CALL(glUniform3f(chunk_pos_uniform_id, world_chunk_x, world_chunk_y,
                             world_chunk_z));
    mesh.vao.draw_arrays(mogl::draw_mode::triangles, 0,
                         mesh.number_of_vertices);
  }
}

void chunk_renderer::on_chunk_update(const chunk_key &key, const chunk &c) {
  auto &mesh = meshes[key];
  build_mesh(mesh, c);
}

void chunk_renderer::on_chunk_insertion(const chunk_key &key, const chunk &c) {
  chunk_mesh m{};
  build_mesh(m, c);
  meshes.insert(std::make_pair(key, std::move(m)));
}

void chunk_renderer::on_chunk_removal(const chunk_key &key) {
  if (auto itr = meshes.find(key) != meshes.end()) {
    meshes.erase(itr);
  }
}

void chunk_renderer::build_mesh(chunk_mesh &mesh, const chunk &c) {
  // build the most naive mesh possible
  buffer_data mesh_data;

  // for each voxel in chunk c
  // generate triangles for each face and stuff it in the appropriate
  // buffer_data
  // then upload completed buffers to the correct vbos
  // TODO(co): the chunk should really expose an iterator that we use to iterate
  // through the voxels. If our underlying chunk uses a map to store voxel
  // information
  // then this is one dumb tripple loop
  for (auto z = 0; z < chunk::depth; ++z) {
    for (auto y = 0; y < chunk::height; ++y) {
      for (auto x = 0; x < chunk::width; ++x) {
        const auto t = c.get(x, y, z);
        if (t == block_type::air) {
          continue;
        }
        if (c.is_face_visible<face::front>(x, y, z)) {
          mesh_data.emplace_back(x, y + 1, z, t);
          mesh_data.emplace_back(x, y, z, t);
          mesh_data.emplace_back(x + 1, y, z, t);

          mesh_data.emplace_back(x + 1, y, z, t);
          mesh_data.emplace_back(x + 1, y + 1, z, t);
          mesh_data.emplace_back(x, y + 1, z, t);
        }
        if (c.is_face_visible<face::back>(x, y, z)) {
          mesh_data.emplace_back(x + 1, y + 1, z + 1, t);
          mesh_data.emplace_back(x + 1, y, z + 1, t);
          mesh_data.emplace_back(x, y, z + 1, t);

          mesh_data.emplace_back(x, y, z + 1, t);
          mesh_data.emplace_back(x, y + 1, z + 1, t);
          mesh_data.emplace_back(x + 1, y + 1, z + 1, t);
        }
        if (c.is_face_visible<face::left>(x, y, z)) {
          mesh_data.emplace_back(x, y + 1, z + 1, t);
          mesh_data.emplace_back(x, y, z + 1, t);
          mesh_data.emplace_back(x, y, z, t);

          mesh_data.emplace_back(x, y, z, t);
          mesh_data.emplace_back(x, y + 1, z, t);
          mesh_data.emplace_back(x, y + 1, z + 1, t);
        }
        if (c.is_face_visible<face::right>(x, y, z)) {
          mesh_data.emplace_back(x + 1, y + 1, z, t);
          mesh_data.emplace_back(x + 1, y, z, t);
          mesh_data.emplace_back(x + 1, y, z + 1, t);

          mesh_data.emplace_back(x + 1, y, z + 1, t);
          mesh_data.emplace_back(x + 1, y + 1, z + 1, t);
          mesh_data.emplace_back(x + 1, y + 1, z, t);
        }
        if (c.is_face_visible<face::top>(x, y, z)) {
          mesh_data.emplace_back(x, y + 1, z + 1, t);
          mesh_data.emplace_back(x, y + 1, z, t);
          mesh_data.emplace_back(x + 1, y + 1, z, t);

          mesh_data.emplace_back(x + 1, y + 1, z, t);
          mesh_data.emplace_back(x + 1, y + 1, z + 1, t);
          mesh_data.emplace_back(x, y + 1, z + 1, t);
        }
        if (c.is_face_visible<face::bottom>(x, y, z)) {
          mesh_data.emplace_back(x, y, z, t);
          mesh_data.emplace_back(x, y, z + 1, t);
          mesh_data.emplace_back(x + 1, y, z + 1, t);

          mesh_data.emplace_back(x + 1, y, z + 1, t);
          mesh_data.emplace_back(x + 1, y, z, t);
          mesh_data.emplace_back(x, y, z, t);
        }
      }
    }
  }

  // upload data to buffers
  mesh.number_of_vertices = mesh_data.size();
  mesh.vbo.data(mesh_data);
  mesh.vao.vertex_attrib_pointer(mesh.vbo, cube_pos_attrib_id, 4,
                                 GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
  mesh.vao.enable_vertex_attrib_array(cube_pos_attrib_id);
}

} // namespace lexov
