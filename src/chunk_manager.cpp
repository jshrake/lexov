#include "chunk_manager.hpp"
#include "chunk_generator.hpp"
#include "chunk_renderer.hpp"
#include <cassert>

namespace lexov {
chunk_manager::chunk_manager(chunk_renderer &cr) : renderer{ cr } {
  for (world_size_t z = 0; z < world_depth; ++z) {
    for (world_size_t y = 0; y < world_height; ++y) {
      for (world_size_t x = 0; x < world_width; ++x) {
        const auto k = chunk_key{x, y, z};
        insert_chunk(k,
                     chunk_generator::make_floating_rock(k));
      }
    }
  }
}

void chunk_manager::insert_chunk(const chunk_key &key, chunk_ptr ptr) {
  // set up chunk neighbors
  const auto x = std::get<0>(key);
  const auto y = std::get<1>(key);
  const auto z = std::get<2>(key);
  // TODO(co): refactor the set neighbor code to not be junk
  // Set front neighbor
  const auto front_itr = all_chunks.find({x, y, z - 1});
  if (front_itr != all_chunks.cend()) {
    const auto neighbor = front_itr->second;
    neighbor->set_neighbor<face::back>(ptr);
    ptr->set_neighbor<face::front>(neighbor);
  }
  // Set back neighbor
  const auto back_itr = all_chunks.find({x, y, z + 1});
  if (back_itr != all_chunks.cend()) {
    const auto neighbor = back_itr->second;
    neighbor->set_neighbor<face::front>(ptr);
    ptr->set_neighbor<face::back>(neighbor);
  }
  // Set left neighbor
  const auto left_itr = all_chunks.find({x - 1, y, z});
  if (left_itr != all_chunks.cend()) {
    const auto neighbor = left_itr->second;
    neighbor->set_neighbor<face::right>(ptr);
    ptr->set_neighbor<face::left>(neighbor);
  }
  // Set right neighbor
  const auto right_itr = all_chunks.find({x + 1, y, z});
  if (right_itr != all_chunks.cend()) {
    const auto neighbor = right_itr->second;
    neighbor->set_neighbor<face::left>(ptr);
    ptr->set_neighbor<face::right>(neighbor);
  }
  // Set top neighbor
  const auto top_itr = all_chunks.find({x, y + 1, z});
  if (top_itr != all_chunks.cend()) {
    const auto neighbor = top_itr->second;
    neighbor->set_neighbor<face::bottom>(ptr);
    ptr->set_neighbor<face::top>(neighbor);
  }
  // Set bottom neighbor
  const auto bottom_itr = all_chunks.find({x, y - 1, z});
  if (bottom_itr != all_chunks.cend()) {
    const auto neighbor = bottom_itr->second;
    neighbor->set_neighbor<face::top>(ptr);
    ptr->set_neighbor<face::bottom>(neighbor);
  }
  renderer.on_chunk_insertion(key, *ptr);
  all_chunks[key] = ptr;
  assert(all_chunks.find(key) != all_chunks.end());
}

void chunk_manager::remove_chunk(const chunk_key &key) {
  if (auto itr = all_chunks.find(key) != all_chunks.end()) {
    all_chunks.erase(itr);
    renderer.on_chunk_removal(key);
  }
}

void chunk_manager::update() {
  for (const auto &itr : all_chunks) {
    if (itr.second->is_dirty()) {
      renderer.on_chunk_update(itr.first, *itr.second);
      itr.second->mark_clean();
    }
  }
}

} // namespace lexov

