#pragma once
#include "types.hpp"
#include <cstddef>
#include <memory>
namespace lexov {

template <local_size_t W, local_size_t H = W, local_size_t D = W> class chunk_base {
public:
  static constexpr local_size_t width = W;
  static constexpr local_size_t height = H;
  static constexpr local_size_t depth = D;
  static constexpr std::size_t volume = W * H * D;

  block_type get(const local_size_t x, const local_size_t y,
                 const local_size_t z) const;

  void set(const local_size_t x, const local_size_t y, const local_size_t z,
           const block_type type);

  bool is_solid(const local_size_t x, const local_size_t y,
                const local_size_t z) const;

  template <face face>
  bool is_face_visible(const local_size_t x, const local_size_t y,
                       const local_size_t z) const;

  template <face face>
  void set_neighbor(const std::shared_ptr<chunk_base const> &neighbor);

  bool is_dirty() const;
  void mark_dirty() const;
  void mark_clean() const;

private:
  virtual block_type get_impl(const local_size_t x, const local_size_t y,
                              const local_size_t z) const = 0;

  virtual void set_impl(const local_size_t x, const local_size_t y,
                        const local_size_t z, const block_type type) = 0;

  virtual bool is_solid_impl(const local_size_t x, const local_size_t y,
                             const local_size_t z) const = 0;

  // Maintain weak pointers to neighboring chunks
  using weak_chunk_ptr = std::weak_ptr<chunk_base const>;
  weak_chunk_ptr front_neighbor;
  weak_chunk_ptr back_neighbor;
  weak_chunk_ptr left_neighbor;
  weak_chunk_ptr right_neighbor;
  weak_chunk_ptr top_neighbor;
  weak_chunk_ptr bottom_neighbor;

  mutable bool dirty{ false };
};

template <local_size_t W, local_size_t H, local_size_t D>
block_type chunk_base<W, H, D>::get(const local_size_t x, const local_size_t y,
                               const local_size_t z) const {
  return get_impl(x, y, z);
}

template <local_size_t W, local_size_t H, local_size_t D>
void chunk_base<W, H, D>::set(const local_size_t x, const local_size_t y,
                         const local_size_t z, const block_type type) {
  set_impl(x, y, z, type);
  static const auto mark_neighbor_dirty = [](const weak_chunk_ptr & ptr) {
    auto neighbor = ptr.lock();
    if (neighbor) {
      neighbor->mark_dirty();
    }
  };
  // If we made a change to a border cube, mark the bordering neighbor as dirty
  if (dirty) {
    if (x == 0) {
      mark_neighbor_dirty(left_neighbor);
    } else if (x == W - 1) {
      mark_neighbor_dirty(right_neighbor);
    }

    if (y == 0) {
      mark_neighbor_dirty(bottom_neighbor);
    } else if (y == H - 1) {
      mark_neighbor_dirty(top_neighbor);
    }

    if (z == 0) {
      mark_neighbor_dirty(front_neighbor);
    } else if (z == D - 1) {
      mark_neighbor_dirty(back_neighbor);
    }
  }
}

template <local_size_t W, local_size_t H, local_size_t D>
bool chunk_base<W, H, D>::is_solid(const local_size_t x, const local_size_t y,
                              const local_size_t z) const {
  return is_solid_impl(x, y, z);
}


template <local_size_t W, local_size_t H, local_size_t D>
template <face face>
bool chunk_base<W, H, D>::is_face_visible(const local_size_t x, const local_size_t y,
                                     const local_size_t z) const {
  if (!is_solid(x, y, z)) {
    return false;
  }
  static const auto check_neighbor = [](
      const weak_chunk_ptr & ptr, const local_size_t x, const local_size_t y,
      const local_size_t z) {
    auto neighbor = ptr.lock();
    if (neighbor) {
      return !neighbor->is_solid(x, y, z);
    } else {
      return true;
    }
  };
  switch (face) {
  case face::front:
    if (z == 0) {
      return check_neighbor(front_neighbor, x, y, D - 1);
    } else {
      return !is_solid(x, y, z - 1);
    }
  case face::back:
    if (z == depth - 1) {
      return check_neighbor(back_neighbor, x, y, 0);
    } else {
      return !is_solid(x, y, z + 1);
    }
  case face::left:
    if (x == 0) {
      return check_neighbor(left_neighbor, W - 1, y, z);
    } else {
      return !is_solid(x - 1, y, z);
    }
  case face::right:
    if (x == width - 1) {
      return check_neighbor(right_neighbor, 0, y, z);
    } else {
      return !is_solid(x + 1, y, z);
    }
  case face::top:
    if (y == height - 1) {
      return check_neighbor(top_neighbor, x, 0, z);
    } else {
      return !is_solid(x, y + 1, z);
    }
  case face::bottom:
    if (y == 0) {
      return check_neighbor(bottom_neighbor, x, height - 1, z);
    } else {
      return !is_solid(x, y - 1, z);
    }
  }
}

template <local_size_t W, local_size_t H, local_size_t D>
template <face face>
void
chunk_base<W, H, D>::set_neighbor(const std::shared_ptr<chunk_base const> &neighbor) {
  switch (face) {
  case face::front:
    front_neighbor = neighbor;
    break;
  case face::back:
    back_neighbor = neighbor;
    break;
  case face::left:
    left_neighbor = neighbor;
    break;
  case face::right:
    right_neighbor = neighbor;
    break;
  case face::top:
    top_neighbor = neighbor;
    break;
  case face::bottom:
    bottom_neighbor = neighbor;
    break;
  }
}

template <local_size_t W, local_size_t H, local_size_t D>
bool chunk_base<W, H, D>::is_dirty() const {
  return dirty;
}

template <local_size_t W, local_size_t H, local_size_t D>
void chunk_base<W, H, D>::mark_dirty() const {
  dirty = true;
}

template <local_size_t W, local_size_t H, local_size_t D>
void chunk_base<W, H, D>::mark_clean() const {
  dirty = false;
}

} // namespace lexov
