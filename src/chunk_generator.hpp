#pragma once
#include "chunk.hpp"
#include "types.hpp"
#include <tuple>

namespace lexov {

namespace chunk_generator {
  chunk_ptr make_solid_chunk(const block_type type);
  chunk_ptr make_random_chunk(const double p);
  chunk_ptr make_pyramid();
  std::tuple<chunk_key, chunk_ptr> make_floating_rock(const chunk_key);
}

} // namespace lexov
