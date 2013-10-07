#pragma once
#include "game.hpp"
#include "camera.hpp"
#include "chunk_generator.hpp"
#include "chunk_manager.hpp"
#include "chunk_renderer.hpp"
#include <memory>
#include <GLFW/glfw3.h>

namespace lexov {
class game : public fixed_timestep_game {
public:
  game(GLFWwindow &window);
  ~game() = default;

private:
  void update(const delta_time &) override;
  void draw() override;
  void pre_update() override;
  bool should_quit() override;

  GLFWwindow &window_;
  std::unique_ptr<camera> camera_;
  std::unique_ptr<chunk_renderer> renderer_;
  std::unique_ptr<chunk_manager> manager_;
};

} // namespace lexov
