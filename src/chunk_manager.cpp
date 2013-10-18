#include "chunk_manager.hpp"
#include "chunk_generator.hpp"
#include "chunk_renderer.hpp"
#include <cassert>
#include <thread>
#include <future>
#include <vector>

namespace lexov {
chunk_manager::chunk_manager(chunk_renderer &cr) : renderer{ cr } {
  std::vector<std::future<std::tuple<chunk_key, chunk_ptr>>> chunk_futures;
  chunk_futures.reserve(world_depth * world_height * world_depth);
  for (world_size_t z = 0; z < world_depth; ++z) {
    for (world_size_t y = 0; y < world_height; ++y) {
      for (world_size_t x = 0; x < world_width; ++x) {
        const auto k = chunk_key{x, y, z};
        chunk_futures.push_back(std::async(std::launch::async, chunk_generator::make_floating_rock, k));
      }
    }
  }
  for (auto &f : chunk_futures) {
    auto res = f.get();
    insert_chunk(std::get<0>(res), std::get<1>(res));
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
    renderer.on_chunk_removal(key);
    all_chunks.erase(itr);
  }
}

void chunk_manager::update(const world_size_t , const world_size_t , const world_size_t ) {
  // Determine chunks that need building based on camera position + lookat
  //  - vector / queue / priority queue of chunk keys
  // add work to the build -> mesh -> insert pipeline
  //  - build chunk, store result in manager (concurrent datastructure for storage)
  //  - mesh chunk, store result in renderer (concurrent datastructure for storage)
  // Determine chunks that need removal
  //  - remove chunks from manager
  //  - remove chunks from renderer
  for (const auto &itr : all_chunks) {
    if (itr.second->is_dirty()) {
      renderer.on_chunk_update(itr.first, *itr.second);
      itr.second->mark_clean();
    }
  }
}
auto chunk_manager::get_total_number_of_solid_blocks() const -> decltype(
    chunk::volume) {
    std::size_t count = 0;
    for (auto &c : all_chunks) {
      std::size_t chunk_solid_count = 0;
      static const auto solid_counter = [&chunk_solid_count](const chunk &c, const local_size_t x, const local_size_t y, const local_size_t z) {
        if (c.is_solid(x, y, z)) {
          ++chunk_solid_count;
        }
      };
      for_each_voxel(*c.second, solid_counter);
      count += chunk_solid_count;
    }
    return count;
  }
} // namespace lexov

