#pragma once
#include "chunk.hpp"
#include "types.hpp"

namespace lexov {

  class chunk_generator {
    public:
    static chunk_ptr make_solid_chunk(const block_type type);
    static chunk_ptr make_random_chunk(const double p);
    static chunk_ptr make_pyramid();
    static chunk_ptr make_floating_rock(const chunk_key &);
  };

} // namespace lexov
