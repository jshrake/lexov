#include "chunk_manager.hpp"
#include "chunk_generator.hpp"
#include "chunk_renderer.hpp"

namespace lexov {
chunk_manager::chunk_manager(chunk_renderer &cr)
    : renderer{ cr } {
      insert_chunk(chunk_key{0, 0, 0}, chunk_generator::make_50_50_chunk(block_type::grass));
    }

void chunk_manager::insert_chunk(const chunk_key &key, const chunk_ptr &ptr) {
  all_chunks.insert(std::make_pair(key, ptr));
  renderer.on_chunk_insertion(key, *ptr);
}

void chunk_manager::remove_chunk(const chunk_key &key) {
  if (auto itr = all_chunks.find(key) != all_chunks.end()) {
    all_chunks.erase(itr);
    renderer.on_chunk_removal(key);
  }
}

void chunk_manager::update() {

}

} // namespace lexov


