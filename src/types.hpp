#pragma once
#include "utility.hpp"
#include <cstdint>
#include <tuple>

namespace lexov {

// represents the different block types of lexov
enum class block_type : std::uint_least8_t {
  air = 0, grass, dirt, water, sand, rock, brick, snow,
};

enum class face : std::uint_least8_t {
  front, back, left, right, top, bottom
};

using local_size_t = std::uint_least8_t;
using world_size_t = long long;
using real = long double;

static constexpr const local_size_t chunk_width  = 32;
static constexpr const local_size_t chunk_height = 32;
static constexpr const local_size_t chunk_depth  = 32;

static constexpr const local_size_t world_width  = 8;
static constexpr const local_size_t world_height = 8;
static constexpr const local_size_t world_depth  = 8;



using chunk_key = std::tuple<world_size_t, world_size_t, world_size_t>;

struct chunk_hash final {
  size_t operator()(const chunk_key &key) const {
    std::size_t seed;
    hash_combine(seed, std::get<0>(key));
    hash_combine(seed, std::get<1>(key));
    hash_combine(seed, std::get<2>(key));
    return seed;
  }
};

struct chunk_hash_equal final {
bool operator()(const chunk_key &lhs, const chunk_key &rhs) const {
  return std::get<0>(lhs) == std::get<0>(rhs) &&
         std::get<1>(lhs) == std::get<1>(rhs) &&
         std::get<2>(lhs) == std::get<2>(rhs);
  }

};
} // namespace lexov
