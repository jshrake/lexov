#include "chunk_renderer.hpp"
#include "camera.hpp"

namespace lexov {

const std::string chunk_renderer::cube_pos_attrib_name = "cube_pos";
const std::string chunk_renderer::chunk_pos_uniform_name = "chunk_pos";
const std::string chunk_renderer::normal_uniform_name = "normal";
const std::string chunk_renderer::camera_pos_uniform_name = "vp_matrix";

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
}

void chunk_renderer::render(const camera &cam) {
  static const auto draw_face = [](const chunk_face_buffer & face) {
    face.vao.draw_arrays(mogl::draw_mode::triangles, 0,
                         face.number_of_vertices);
  };

  auto tmp = shader_program.scope_bind();
  // Set the camera matrix
  CHECKED_CALL(glUniformMatrix4fv(camera_pos_uniform_id, 1, GL_FALSE,
                     cam.get_view_projection()));
  for (const auto &itr : meshes) {
    const auto &pos = itr.first;
    const auto &mesh = itr.second;
    // Set the chunk position
    CHECKED_CALL(glUniform3f(chunk_pos_uniform_id, std::get<0>(pos), std::get<1>(pos),
                std::get<2>(pos)));
    // Determine which faces we should draw, if any, for this chunk
    // Set the normal uniform before drawing
    if (cam.is_point_visible(chunk_width / 2, chunk_height, chunk_depth / 2)) {
      CHECKED_CALL(glUniform3fv(normal_uniform_id, 1, &top_normal[0]));
      draw_face(mesh.top_face);
    }
    if (cam.is_point_visible(chunk_width / 2, 0, chunk_depth / 2)) {
      CHECKED_CALL(glUniform3fv(normal_uniform_id, 1, &bottom_normal[0]));
      draw_face(mesh.bottom_face);
    }
    if (cam.is_point_visible(0, chunk_height / 2, chunk_depth / 2)) {
      CHECKED_CALL(glUniform3fv(normal_uniform_id, 1, &left_normal[0]));
      draw_face(mesh.left_face);
    }
    if (cam.is_point_visible(chunk_width, chunk_height / 2, chunk_depth / 2)) {
      CHECKED_CALL(glUniform3fv(normal_uniform_id, 1, &right_normal[0]));
      draw_face(mesh.right_face);
    }
    if (cam.is_point_visible(chunk_width / 2, chunk_height / 2, 0)) {
      CHECKED_CALL(glUniform3fv(normal_uniform_id, 1, &front_normal[0]));
      draw_face(mesh.front_face);
    }
    if (cam.is_point_visible(chunk_width / 2, chunk_height / 2, chunk_depth)) {
      CHECKED_CALL(glUniform3fv(normal_uniform_id, 1, &back_normal[0]));
      draw_face(mesh.back_face);
    }
  }
}

void chunk_renderer::on_chunk_update(const chunk_key &key, const chunk &c) {
  auto &mesh = meshes[key];
  build_mesh(mesh, c);
}

void chunk_renderer::on_chunk_insertion(const chunk_key &key, const chunk &c) {
  auto itr = meshes.emplace(std::make_pair(key, chunk_mesh{}));
  build_mesh(itr.first->second, c);
}

void chunk_renderer::on_chunk_removal(const chunk_key &key) {
  if (auto itr = meshes.find(key) != meshes.end()) {
    meshes.erase(itr);
  }
}

void chunk_renderer::build_mesh(chunk_mesh &mesh, const chunk &c) {
  // build the most naive mesh possible
  buffer_data front_data;
  buffer_data back_data;
  buffer_data left_data;
  buffer_data right_data;
  buffer_data top_data;
  buffer_data bottom_data;

  // for each voxel in chunk c
  // generate triangles for each face and stuff it in the appropriate
  // buffer_data
  // then upload completed buffers to the correct vbos
  // TODO(co): the chunk should really expose an iterator that we use to iterate
  // through the voxels. If our underlying chunk uses a map to store voxel
  // information
  // then this is one dumb tripple loop
  for (auto z = 0; z < chunk_depth; ++z) {
    for (auto y = 0; y < chunk_height; ++y) {
      for (auto x = 0; x < chunk_width; ++x) {
        const auto t = c.get(x, y, z);
        if (t == block_type::air) {
          continue;
        }
        if (c.is_face_visible<face::front>(x, y, z)) {
          front_data.emplace_back(x, y + 1, z, t);
          front_data.emplace_back(x, y, z, t);
          front_data.emplace_back(x + 1, y, z, t);

          front_data.emplace_back(x + 1, y, z, t);
          front_data.emplace_back(x + 1, y + 1, z, t);
          front_data.emplace_back(x, y + 1, z, t);
        }
        if (c.is_face_visible<face::back>(x, y, z)) {
          back_data.emplace_back(x + 1, y + 1, z + 1, t);
          back_data.emplace_back(x + 1, y, z + 1, t);
          back_data.emplace_back(x, y, z + 1, t);

          back_data.emplace_back(x, y, z + 1, t);
          back_data.emplace_back(x, y + 1, z + 1, t);
          back_data.emplace_back(x + 1, y + 1, z + 1, t);
        }
        if (c.is_face_visible<face::left>(x, y, z)) {
          left_data.emplace_back(x, y + 1, z + 1, t);
          left_data.emplace_back(x, y, z + 1, t);
          left_data.emplace_back(x, y, z, t);

          left_data.emplace_back(x, y, z, t);
          left_data.emplace_back(x, y + 1, z, t);
          left_data.emplace_back(x, y + 1, z + 1, t);
        }
        if (c.is_face_visible<face::right>(x, y, z)) {
          right_data.emplace_back(x + 1, y + 1, z, t);
          right_data.emplace_back(x + 1, y, z, t);
          right_data.emplace_back(x + 1, y, z + 1, t);

          right_data.emplace_back(x + 1, y, z + 1, t);
          right_data.emplace_back(x + 1, y + 1, z + 1, t);
          right_data.emplace_back(x + 1, y + 1, z, t);
        }
        if (c.is_face_visible<face::top>(x, y, z)) {
          top_data.emplace_back(x, y + 1, z + 1, t);
          top_data.emplace_back(x, y + 1, z, t);
          top_data.emplace_back(x + 1, y + 1, z, t);

          top_data.emplace_back(x + 1, y + 1, z, t);
          top_data.emplace_back(x + 1, y + 1, z + 1, t);
          top_data.emplace_back(x, y + 1, z + 1, t);
        }
        if (c.is_face_visible<face::bottom>(x, y, z)) {
          bottom_data.emplace_back(x, y, z, t);
          bottom_data.emplace_back(x, y + 1, z, t);
          bottom_data.emplace_back(x + 1, y, z + 1, t);

          bottom_data.emplace_back(x + 1, y, z + 1, t);
          bottom_data.emplace_back(x + 1, y, z, t);
          bottom_data.emplace_back(x, y, z, t);
        }
      }
    }
  }

  // upload data to buffers
  upload_vertex_data(mesh.front_face, front_data);
  upload_vertex_data(mesh.back_face, back_data);
  upload_vertex_data(mesh.left_face, left_data);
  upload_vertex_data(mesh.right_face, right_data);
  upload_vertex_data(mesh.top_face, top_data);
  upload_vertex_data(mesh.bottom_face, bottom_data);
}

void chunk_renderer::upload_vertex_data(chunk_face_buffer &cfb,
                                        const buffer_data &d) {
  cfb.number_of_vertices = d.size();
  cfb.vbo.data(d);
  cfb.vao.vertex_attrib_pointer(cfb.vbo, cube_pos_attrib_id, 4,
                                GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
  cfb.vao.enable_vertex_attrib_array(cube_pos_attrib_id);
}

} // namespace lexov
