#include "game.hpp"

void fixed_timestep_game::load_content() {}

void fixed_timestep_game::pre_update(const delta_time &) {}

void fixed_timestep_game::post_update(const double) {}

void fixed_timestep_game::unload_content() {}

void fixed_timestep_game::start() {
  load_content();
  loop();
  unload_content();
}

void fixed_timestep_game::loop() {
  constexpr auto dt = delta_time{ 1 };
  auto previous_time = clock::now();
  delta_time accumulator{ 0 };
  while (!should_quit()) {
    const auto current_time = clock::now();
    const auto frame_time = current_time - previous_time;
    previous_time = current_time;
    accumulator += frame_time;
    pre_update(dt);
    while (accumulator > dt) {
      update(dt);
      accumulator -= dt;
    }
    const auto alpha = accumulator / dt;
    post_update(alpha);
    draw();
  }
}
