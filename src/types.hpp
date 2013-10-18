#pragma once
#include "utility.hpp"
#include <array>
#include <cstdint>
#include <tuple>

namespace lexov {

// represents the different block types of lexov
enum class block_type : std::uint_least8_t {
  air = 0, grass, dirt, water, stone, count
};

enum class face : std::uint_least8_t {
  front, back, left, right, top, bottom
};

using local_size_t = std::uint_least8_t;
using world_size_t = long long;
using real = long double;

constexpr const local_size_t chunk_width = 16;
constexpr const local_size_t chunk_height = 128;
constexpr const local_size_t chunk_depth = 16;

constexpr const local_size_t half_chunk_width = chunk_width / 2;
constexpr const local_size_t half_chunk_height = chunk_height / 2;
constexpr const local_size_t half_chunk_depth = chunk_depth / 2;

constexpr const local_size_t chunk_diameter =
    isqrt(half_chunk_width * half_chunk_width +
          half_chunk_height * half_chunk_height +
          half_chunk_depth * half_chunk_depth);

constexpr const local_size_t world_width = 32;
constexpr const local_size_t world_height = 3;
constexpr const local_size_t world_depth = 32;

using chunk_key = std::tuple<world_size_t, world_size_t, world_size_t>;

struct chunk_hash final {
  size_t operator()(const chunk_key &key) const {
    std::size_t seed = 0;
    hash_combine(seed, std::get<0>(key));
    hash_combine(seed, std::get<1>(key));
    hash_combine(seed, std::get<2>(key));
    return seed;
  }
};

struct chunk_hash_equal final {
bool operator()(const chunk_key &lhs, const chunk_key &rhs) const {
  return lhs == rhs; 
  }
};

} // namespace lexov
