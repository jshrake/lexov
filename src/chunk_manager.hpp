#pragma once
#include "types.hpp"
#include "chunk.hpp"
#include "utility.hpp"
#include <cstdint>
#include <list>
#include <memory>
#include <tuple>
#include <unordered_map>

namespace lexov {

class chunk_renderer;

class chunk_manager {
public:
  chunk_manager(chunk_renderer &cr);
  void update();

private:
  void insert_chunk(const chunk_key &key, chunk_ptr ptr); 
  void remove_chunk(const chunk_key &key);
  chunk_renderer &renderer;

  using chunk_map = std::unordered_map<chunk_key, chunk_ptr, chunk_hash, chunk_hash_equal>;
  using weak_chunk_map = std::unordered_map<chunk_key, weak_chunk_ptr, chunk_hash, chunk_hash_equal>;

  // Owns all chunks
  chunk_map all_chunks{};
};
} // namespace
