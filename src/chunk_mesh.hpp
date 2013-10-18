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

  struct chunk_mesh {
    mogl::vertex_array_object vao;
    mogl::stream_array_buffer vbo;
    std::size_t number_of_vertices;
  };

  using buffer_data = std::vector<voxel_vertex>;
} // namespace lexov
