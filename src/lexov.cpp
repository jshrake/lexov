#include "lexov.hpp"
#include <mogl/mogl.hpp>
#include <GLFW/glfw3.h>

namespace lexov {

game::game(GLFWwindow &window) : window_{ window } {
  // set up game camera
  glfwGetWindowSize(&window_, &window_width, &window_height);
  camera_properties cp;
  cp.aspect_ratio = window_width / (float)window_height;
  cp.eye = { { 40.0f, 40.0f, 40.0f } };
  cp.look_at = { { chunk_width / 2.0f, chunk_height / 2.0f,
                   chunk_depth / 2.0f } };
  cp.up = { { 0.0f, 1.0f, 0.0f } };
  cp.right = { { 1.0f, 0.0f, 0.0f } };
  cp.fov = 80.0f;
  cp.z_near = 0.01f;
  cp.z_far = 1000.0f;
  cp.rotation_rate = 1.0f;
  cp.zoom_rate = 0.5f;
  camera_ = std::unique_ptr<camera>{ new camera{ cp } };

  // Set up opengl shader program for the renderer
  mogl::program p{ mogl::vertex_shader::from_file("chunk.vs"),
                   mogl::fragment_shader::from_file("chunk.fs") };
  renderer_ =
      std::unique_ptr<chunk_renderer>{ new chunk_renderer{ std::move(p) } };

  // Initialize the chunk manager
  manager_ = std::unique_ptr<chunk_manager>{ new chunk_manager{ *renderer_ } };
  glfwSetInputMode(&window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetCursorPos(&window_, window_height/2.0f, window_width/2.0f);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(135.0/255.0f, 100/255.0f, 250/255.0f, 1.0f);
}

void game::load_content() {}

void game::update(const delta_time &) { manager_->update(); }

void game::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  renderer_->render(*camera_);
  glfwSwapBuffers(&window_);
}

void game::pre_update(const delta_time &dt) {
  glfwPollEvents();
  auto dur = dt.count() * 0.5f;
  if (glfwGetKey(&window_, GLFW_KEY_A) == GLFW_PRESS) {
    camera_->move_right(-dur);
  }
  if (glfwGetKey(&window_, GLFW_KEY_D) == GLFW_PRESS) {
    camera_->move_right(dur);
  }
  if (glfwGetKey(&window_, GLFW_KEY_W) == GLFW_PRESS) {
    camera_->move_forward(dur);
  }
  if (glfwGetKey(&window_, GLFW_KEY_S) == GLFW_PRESS) {
    camera_->move_forward(-dur);
  }
  if (glfwGetKey(&window_, GLFW_KEY_Q) == GLFW_PRESS) {
    camera_->move_up(dur);
  }
  if (glfwGetKey(&window_, GLFW_KEY_E) == GLFW_PRESS) {
    camera_->move_up(-dur);
  }
  if (glfwGetKey(&window_, GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(&window_, true);
  }

  if (glfwGetMouseButton(&window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
  const float mouseSensitivity = 0.005f;
  double mouseX, mouseY;
  glfwGetCursorPos(&window_, &mouseX, &mouseY);
   
  camera_->offset_orientation(mouseSensitivity * (mouseX - window_width/2.0f),
                            mouseSensitivity * (mouseY - window_height/2.0f));
  } else {
  glfwSetCursorPos(&window_, window_width/2.0f, window_height/2.0f);
  }
  if (glfwGetKey(&window_, GLFW_KEY_0) == GLFW_PRESS) {
    static bool flag = true;
    if (flag) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    flag = !flag;
  }
}

bool game::should_quit() { return glfwWindowShouldClose(&window_); }
};
