#pragma once
#include "chunk_base.hpp"
#include "types.hpp"
#include <array>

namespace lexov {

template <local_size_t W, local_size_t H = W, local_size_t D = W>
class array_chunk : public chunk_base<W, H, D> {
public:
  using chunk_base_whd = chunk_base<W, H, D>;

private:
  using chunk_data = std::array<block_type, chunk_base_whd::volume>;
  chunk_data data;

  block_type get_impl(const local_size_t x, const local_size_t y,
                      const local_size_t z) const override;
  void set_impl(const local_size_t x, const local_size_t y, const local_size_t z,
                const block_type type) override;
  bool is_solid_impl(const local_size_t x, const local_size_t y,
                     const local_size_t z) const override;
};

template <local_size_t W, local_size_t H, local_size_t D>
block_type
array_chunk<W, H, D>::get_impl(const local_size_t x, const local_size_t y,
                               const local_size_t z) const {
  return data[x + H * y + H * D * z];
}

template <local_size_t W, local_size_t H, local_size_t D>
void array_chunk<W, H, D>::set_impl(const local_size_t x, const local_size_t y,
                                    const local_size_t z,
                                    const block_type type) {
  auto &current_type = data[x + H * y + H * D * z];
  if (current_type != type) {
    current_type = type;
    chunk_base_whd::mark_dirty();
  }
}

template <local_size_t W, local_size_t H, local_size_t D>
bool array_chunk<W, H, D>::is_solid_impl(const local_size_t x,
                                         const local_size_t y,
                                         const local_size_t z) const {
  return data[x + H * y + H * D * z] != block_type::air;
}

} // namespace lexov
