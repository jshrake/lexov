#pragma once
#include <chrono>

class fixed_timestep_game {
public:
using clock = std::chrono::high_resolution_clock;
using delta_time = std::chrono::duration<double, std::ratio<1, 60>>;
  virtual ~fixed_timestep_game() = default;

  void start(); 

protected:
  virtual void load_content();

  virtual void pre_update(const delta_time &);
  virtual void update(const delta_time &) = 0;
  virtual void post_update(const double);

  virtual void draw() = 0;

  virtual void unload_content();
  virtual bool should_quit() = 0;

  virtual void loop();

};
