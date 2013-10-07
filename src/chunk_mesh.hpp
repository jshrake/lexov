#pragma once
#include "types.hpp"
#include <mogl/mogl.hpp>
#include <array>
#include <vector>

namespace lexov {

  struct voxel_vertex {
    voxel_vertex(GLubyte x, GLubyte y, GLubyte z, block_type t)
        : x{ x }, y{ y }, z{ z }, t{ static_cast<GLubyte>(t) } {}
    GLubyte x;
    GLubyte y;
    GLubyte z;
    GLubyte t;
  };

  struct chunk_face_buffer {
    mogl::vertex_array_object vao;
    mogl::stream_array_buffer vbo;
    std::size_t number_of_vertices;
  };

  struct chunk_mesh {
    chunk_face_buffer front_face;
    chunk_face_buffer back_face;
    chunk_face_buffer left_face;
    chunk_face_buffer right_face; 
    chunk_face_buffer top_face; 
    chunk_face_buffer bottom_face;
  };

  static constexpr const std::array<GLfloat, 3> front_normal = { { 0.0f, 0.0f,
                                                                   1.0f } };
  static constexpr const std::array<GLfloat, 3> back_normal = { { 0.0f, 0.0f,
                                                                  -1.0f } };
  static constexpr const std::array<GLfloat, 3> left_normal = { { -1.0f, 0.0f,
                                                                  0.0f } };
  static constexpr const std::array<GLfloat, 3> right_normal = { { 1.0f, 1.0f,
                                                                   0.0f } };
  static constexpr const std::array<GLfloat, 3> top_normal = { { 0.0f, 1.0f,
                                                                 0.0f } };
  static constexpr const std::array<GLfloat, 3> bottom_normal = { { 0.0f, -1.0f,
                                                                    0.0f } };

  using buffer_data = std::vector<voxel_vertex>;
} // namespace lexov
