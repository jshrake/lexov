#include "chunk_generator.hpp"
#include <memory>
#include <random>

namespace lexov {
  chunk_ptr chunk_generator::make_solid_chunk(const block_type type) {
    auto shared_chunk = std::make_shared<chunk>();
    const static auto fill = [&type](chunk &c, const local_size_t x,
                                     const local_size_t y,
                                     const local_size_t z) {
      c.set(x, y, z, type);
    };
    for_each_voxel(*shared_chunk, fill);
    return shared_chunk;
  };

  chunk_ptr chunk_generator::make_50_50_chunk(const block_type type) {
    std::random_device rd;
    std::default_random_engine e{rd()}; 
    const static auto fill = [&type, &e](chunk &c, const local_size_t x, const local_size_t y, const local_size_t z) {
      float rand = std::generate_canonical<float, 1>(e);
      if (rand > 0.95f) {
        c.set(x, y, z, type);
      }
    };
    auto shared_chunk = std::make_shared<chunk>();
    for_each_voxel(*shared_chunk, fill);
    return shared_chunk;
  }
} // namespace lexov
