#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <frame_render.h>
#include <frame_update.h>
#include <init.h>
#include <log.h>
#include <scene.h>

void main_loop(puma::scene &s) {
  static uint64_t begin_time = glfwGetTimerValue();

  auto w = glfwGetCurrentContext();

  while (!glfwWindowShouldClose(w)) {
    render::begin_frame(begin_time, s);
    render::render_viewport(s);
    render::render_window_gui(s);
    update::per_frame_update(s);
    render::end_frame(w, begin_time);
  }
}

int main() {
  log::init();
  auto glfw_win = init::init_all("tinyCAD");
  puma::scene s;
  main_loop(s);

  init::cleanup();
}
