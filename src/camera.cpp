#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace lexov {

camera::camera(camera_properties properties) : properties_(properties) {
  update_projection_matrix();
  update_view_matrix();
  update_view_projection_matrix();
}

const std::array<float,3> &camera::get_position() const { return properties_.eye; }

const std::array<float, 3> &camera::get_look_direction() const {
  return properties_.look_at;
}

const float *camera::get_view_projection() const { 
  if (view_dirty || projection_dirty) {
    if (view_dirty) {
      update_view_matrix();
      view_dirty = false;
    }
    if (projection_dirty) {
      update_projection_matrix();
      projection_dirty = false;
    }
    update_view_projection_matrix();
  }
  return glm::value_ptr(view_projection);
}

void camera::set_position(const float x, const float y, const float z) {
  view_dirty = true;
  properties_.eye = {{x, y, z}};
}

void camera::look_at(const float x, const float y, const float z) {
  view_dirty = true;
  properties_.look_at = {{x, y, z}};
}

void camera::on_viewport_resize(const std::size_t width, const std::size_t height) {
  projection_dirty = true;
  properties_.aspect_ratio = width / static_cast<float>(height);
}

bool camera::is_point_visible(const float , const float , const float ) const {
  // TODO(co): implement
  return true;
}

void camera::update_projection_matrix() const {
  projection = glm::perspective(properties_.fov, properties_.aspect_ratio, properties_.z_near, properties_.z_far);
}

void camera::update_view_matrix() const {
  view = glm::lookAt(array_to_vec3(properties_.eye), array_to_vec3(properties_.look_at), array_to_vec3(properties_.up));
}

void camera::update_view_projection_matrix() const {
  view_projection = projection * view;
}
} // namespace lexov
