#pragma once
#include "chunk.hpp"
#include "types.hpp"

namespace lexov {

  class chunk_generator {
    public:
    static chunk_ptr make_solid_chunk(const block_type type);
    static chunk_ptr make_50_50_chunk(const block_type type);
  };

} // namespace lexov
