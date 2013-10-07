#pragma once
#include <glm/glm.hpp>
#include <array>

namespace lexov {

struct camera_properties {
  std::array<float, 3> eye;
  std::array<float, 3> up;
  std::array<float, 3> right;
  std::array<float, 3> look_at;
  float fov;
  float aspect_ratio;
  float z_near;
  float z_far;
  float rotation_rate;
  float zoom_rate;
};

class camera {
public:
  camera(camera_properties properties);
  const std::array<float, 3> &get_position() const;
  const std::array<float, 3> &get_look_direction() const;
  const float *get_view_projection() const;
  void set_position(const float x, const float y, const float z);
  void look_at(const float x, const float y, const float z);
  void on_viewport_resize(const std::size_t width, const std::size_t height);
  bool is_point_visible(const float x, const float y, const float z) const;

private:
  void update_projection_matrix() const;
  void update_view_matrix() const;
  void update_view_projection_matrix() const;
  camera_properties properties_;
  mutable glm::mat4 view;
  mutable glm::mat4 projection;
  mutable glm::mat4 view_projection;
  mutable bool projection_dirty{false};
  mutable bool view_dirty{false};
};

inline glm::vec3 array_to_vec3(const std::array<float, 3> a) {
  return glm::vec3{a[0], a[1], a[2]};
}


} // namespace lexov
