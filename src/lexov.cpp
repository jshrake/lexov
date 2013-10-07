#include "lexov.hpp"
#include <mogl/mogl.hpp>
#include <GLFW/glfw3.h>

namespace lexov {

game::game(GLFWwindow &window) : window_{ window } {
  // set up game camera
  int window_width = 0, window_height = 0;
  glfwGetWindowSize(&window_, &window_width, &window_height);
  camera_properties cp;
  cp.aspect_ratio = window_width / (float)window_height;
  cp.eye = {{ 40.0f, 40.0f, 40.0f }};
  cp.look_at = {{ 0.0f, 0.0f, 0.0f }};
  cp.up = {{ 0.0f, 1.0f, 0.0f}};
  cp.right = {{ 1.0f, 0.0f, 0.0f }};
  cp.fov = 80.0f;
  cp.z_near = 0.01f;
  cp.z_far = 1000.0f;
  cp.rotation_rate = 45.0f;
  cp.zoom_rate = 1.0f;
  camera_ = std::unique_ptr<camera>{ new camera{ cp} };

  // Set up opengl shader program for the renderer
  mogl::program p{ mogl::vertex_shader::from_file("chunk.vs"),
                   mogl::fragment_shader::from_file("chunk.fs") };
  renderer_ =
      std::unique_ptr<chunk_renderer>{ new chunk_renderer{ std::move(p) } };


  // Initialize the chunk manager
  manager_ = std::unique_ptr<chunk_manager>{ new chunk_manager{ *renderer_ } };
}

void game::update(const delta_time &) {
  manager_->update();
}

void game::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  renderer_->render(*camera_);
  glfwSwapBuffers(&window_);
}

void game::pre_update() {
  glfwPollEvents();
}

bool game::should_quit() {
  return glfwWindowShouldClose(&window_);
}

};
