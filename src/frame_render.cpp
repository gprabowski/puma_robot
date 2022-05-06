#include <frame_render.h>

#include <frame_state.h>
#include <frame_update.h>
#include <framebuffer.h>
#include <gui.h>
#include <input_handlers.h>
#include <utils.h>

namespace hnd = handlers;

namespace render {

void clean_render() {
  static ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
  glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
               clear_color.z * clear_color.w, clear_color.w);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void begin_frame(uint64_t &b, puma::scene &s) {
  clean_render();
  b = glfwGetTimerValue();

  gui::start_frame();
  ImGui::Begin("Viewport");
  hnd::process_input(s);
  ImGui::End();
  update::refresh_ubos();
}

void end_frame(GLFWwindow *w, uint64_t &begin_time) {
  uint64_t end_time = glfwGetTimerValue();
  gui::end_frame();
  frame_state::last_cpu_frame =
      static_cast<double>(end_time - begin_time) * 1000.f / frame_state::freq;

  glfwSwapBuffers(w);
  glfwPollEvents();
}

void render_window_gui(puma::scene &s) {
  gui::render_performance_window();
  gui::render_general_settings();
  gui::render_scene_gui(s);
}

void render_viewport(puma::scene &s) {
  // display
  static auto &fb = framebuffer::get();
  static auto &desc = fb.get_desc();
  ImGui::Begin("Viewport");
  update::setup_globals(ImGui::GetContentRegionAvail());
  const auto region = ImGui::GetContentRegionAvail();

  if (desc.width != region.x || desc.height != region.y) {
    desc.width = region.x;
    desc.height = region.y;
    fb.invalidate();
  }

  // render offscreen
  fb.bind();
  glViewport(0, 0, frame_state::content_area.x, frame_state::content_area.y);
  clean_render();
  s.draw();
  fb.unbind();

  GLuint t = fb.get_color_att();
  ImGui::Image((void *)(uint64_t)t, region, {0, 1}, {1, 0});

  ImGui::End();
}
} // namespace render
