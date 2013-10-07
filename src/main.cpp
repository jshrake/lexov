#include <mogl/mogl.hpp>
#include <GLFW/glfw3.h>
#include "lexov.hpp"
#include <iostream>

int main() {
  try {
    auto width = 640, height = 420;
    auto title = "lexov!";
    auto fullscreen = false;

    if (!glfwInit()) {
      throw std::runtime_error{ "Failed to initialize glfw!" };
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    auto window = glfwCreateWindow(
        width, height, title, fullscreen ? glfwGetPrimaryMonitor() : nullptr,
        nullptr);

    if (!window) {
      glfwTerminate();
      throw std::runtime_error{ "Failed to create glfw window!" };
    }

    glfwMakeContextCurrent(window);

    mogl::initialize();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    lexov::game game{ *window };
    game.start();
  }
  catch (std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
  }
}
