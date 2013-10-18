#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace lexov {

camera::camera(camera_properties properties) : properties_(properties) {
  set_position(properties.eye[0], properties.eye[1], properties.eye[2]);
  look_at(properties.look_at[0], properties.look_at[1], properties.look_at[2]);
}

std::array<float, 3> camera::get_position() const {
  return vec3_to_array(position);
}

void camera::set_position(const float x, const float y, const float z) {
  view_dirty = true;
  position = glm::vec3{ x, y, z };
}

void camera::offset_position(const glm::vec3 &p) {
  view_dirty = true;
  position += p;
}

glm::mat4 camera::attitude() const {
  glm::mat4 orientation;
  orientation = glm::rotate(orientation, elevation_deg, glm::vec3(1, 0, 0));
  orientation = glm::rotate(orientation, azimuth_deg, glm::vec3(0, 1, 0));
  return orientation;
}
void camera::offset_orientation(const float az, const float el) {
  view_dirty = true;
  azimuth_deg += az;
  elevation_deg += el;
  clamp_az_el();
}

float *camera::get_view_projection() const {
  update_view_projection_matrix();
  return glm::value_ptr(view_projection);
}

float *camera::get_view_matrix() const {
  update_view_matrix();
  return glm::value_ptr(view);
}

void camera::move_forward(const float dz) { offset_position(dz * forward()); }

void camera::move_right(const float dx) { offset_position(dx * right()); }

void camera::move_up(const float dy) {
  offset_position(dy * glm::vec3(0, 1, 0));
}

glm::vec3 camera::forward() const {
  glm::vec4 forward = glm::inverse(attitude()) * glm::vec4(0, 0, -1, 1);
  return glm::vec3(forward);
}

glm::vec3 camera::right() const {
  glm::vec4 right = glm::inverse(attitude()) * glm::vec4(1, 0, 0, 1);
  return glm::vec3(right);
}

glm::vec3 camera::up() const {
  glm::vec4 up = glm::inverse(attitude()) * glm::vec4(0, 1, 0, 1);
  return glm::vec3(up);
}

void camera::look_at(const float x, const float y, const float z) {
  view_dirty = true;
  glm::vec3 direction = glm::normalize(glm::vec3{
    x, y, z
  } -
                                       position);
  elevation_deg = asinf(-direction.y) * 180 / M_PI;
  azimuth_deg = -(atan2f(-direction.x, -direction.z)) * 180 / M_PI;
  clamp_az_el();
}

void camera::on_viewport_resize(const std::size_t width,
                                const std::size_t height) {
  projection_dirty = true;
  properties_.aspect_ratio = width / static_cast<float>(height);
}

void camera::update_projection_matrix() const {
  projection = glm::perspective(properties_.fov, properties_.aspect_ratio,
                                properties_.z_near, properties_.z_far);
  projection_dirty = false;
}

void camera::update_view_matrix() const {
  view = attitude() * glm::translate(glm::mat4(), -position);
  view_dirty = false;
}

void camera::update_view_projection_matrix() const {
  if (projection_dirty || view_dirty) {
    if (projection_dirty) {
      update_projection_matrix();
    }
    if (view_dirty) {
      update_view_matrix();
    }
    view_projection = projection * view;
  }
}

void camera::clamp_az_el() {
  constexpr static const float max_elevation = 85.0f;
  azimuth_deg = fmodf(azimuth_deg, 360.0f);
  // fmodf can return negative values, but this will make them all positive
  if (azimuth_deg < 0.0f)
    azimuth_deg += 360.0f;

  if (elevation_deg > max_elevation)
    elevation_deg = max_elevation;
  else if (elevation_deg < -max_elevation)
    elevation_deg = -max_elevation;
}

} // namespace lexov
