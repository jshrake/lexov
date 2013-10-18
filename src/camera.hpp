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
inline glm::vec3 array_to_vec3(const std::array<float, 3> &a) {
  return glm::vec3{a[0], a[1], a[2]};
}

inline std::array<float, 3> vec3_to_array(const glm::vec3 &v) {
  return {{v.x, v.y, v.z}};
}

inline std::array<float, 4> vec4_to_array(const glm::vec4 &v) {
  return {{v.x/v.w, v.y/v.w, v.z/v.w, v.w}};
}
class camera {
public:
  camera(camera_properties properties);
  std::array<float, 3> get_position() const;
  void set_position(const float x, const float y, const float z);
  void move_forward(const float dz);
  void move_right(const float dx);
  void move_up(const float dy);
  void offset_orientation(const float az, const float el);
  void look_at(const float x, const float y, const float z);
  float *get_view_projection() const;
  float *get_view_matrix() const;
  void on_viewport_resize(const std::size_t width, const std::size_t height);

  template <class T>
  std::array<float, 4>
  world_to_clip(const std::array<T, 3> &position) const {
    glm::vec4 p{position[0], position[1], position[2], 1.0f};
    update_view_projection_matrix();
    return vec4_to_array(view_projection * p);
  }

private:
  void offset_position(const glm::vec3 &p);
  glm::mat4 attitude() const;
  glm::vec3 forward() const;
  glm::vec3 right() const;
  glm::vec3 up() const;
  void clamp_az_el();
  void update_view_matrix() const;
  void update_projection_matrix() const;
  void update_view_projection_matrix() const;
  camera_properties properties_;
  glm::vec3 position{1.0f, 1.0f, 1.0f};
  float azimuth_deg{0.0f};
  float elevation_deg{0.0f};

  mutable glm::mat4 view{};
  mutable glm::mat4 projection{};
  mutable glm::mat4 view_projection{};
  mutable bool projection_dirty{true};
  mutable bool view_dirty{true};
};




} // namespace lexov
