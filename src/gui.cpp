#include "imgui.h"
#include "imgui_internal.h"
#include <misc/cpp/imgui_stdlib.h>

#include <implot/implot.h>

#include <algorithm>
#include <optional>
#include <string>
#include <utility>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <frame_state.h>
#include <gl_object.h>
#include <gui.h>
#include <utils.h>

namespace gui {

template <class Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
  return static_cast<std::underlying_type_t<Enum>>(e);
}

static auto vector_getter = [](void *vec, int idx, const char **out_text) {
  auto &vector = *static_cast<std::vector<std::string> *>(vec);
  if (idx < 0 || idx >= static_cast<int>(vector.size())) {
    return false;
  }
  *out_text = vector.at(idx).c_str();
  return true;
};

void setup_gui(std::shared_ptr<GLFWwindow> &w) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  // io.ConfigViewportsNoAutoMerge = true;
  // io.ConfigViewportsNoTaskBarIcon = true;

  ImGui::StyleColorsDark();

  // when viewports are enables we tweak WindowRounding/WIndowBg so platform
  // windows can look identical to regular ones
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(w.get(), true);
  ImGui_ImplOpenGL3_Init("#version 460");

  // fonts
  io.FontDefault = io.Fonts->AddFontFromFileTTF(
      //"fonts/opensans/static/OpenSans/OpenSans-Regular.ttf",
      "fonts/jbmono/fonts/ttf/JetBrainsMono-Regular.ttf", 18.0f);
}

void cleanup_gui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
}

// utility structure for realtime plot
struct ScrollingBuffer {
  int MaxSize;
  int Offset;
  ImVector<ImVec2> Data;
  ScrollingBuffer(int max_size = 2000) {
    MaxSize = max_size;
    Offset = 0;
    Data.reserve(MaxSize);
  }
  void AddPoint(float x, float y) {
    if (Data.size() < MaxSize)
      Data.push_back(ImVec2(x, y));
    else {
      Data[Offset] = ImVec2(x, y);
      Offset = (Offset + 1) % MaxSize;
    }
  }
  void Erase() {
    if (Data.size() > 0) {
      Data.shrink(0);
      Offset = 0;
    }
  }
};

// utility structure for realtime plot
struct RollingBuffer {
  float Span;
  ImVector<ImVec2> Data;
  RollingBuffer() {
    Span = 10.0f;
    Data.reserve(2000);
  }
  void AddPoint(float x, float y) {
    float xmod = fmodf(x, Span);
    if (!Data.empty() && xmod < Data.back().x)
      Data.shrink(0);
    Data.push_back(ImVec2(xmod, y));
  }
};

void ShowDemo_RealtimePlots() {
  static ScrollingBuffer sdata1;
  static RollingBuffer rdata1;
  static float t = 0;
  t += ImGui::GetIO().DeltaTime;
  sdata1.AddPoint(t, frame_state::last_cpu_frame);
  rdata1.AddPoint(t, ImGui::GetIO().Framerate);

  static float history = 10.0f;
  ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");
  rdata1.Span = history;

  static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

  if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 150))) {
    ImPlot::SetupAxes(NULL, NULL, flags, flags);
    ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 16.f);
    ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
    ImPlot::PlotShaded("Frame time in ms", &sdata1.Data[0].x, &sdata1.Data[0].y,
                       sdata1.Data.size(), -INFINITY, sdata1.Offset,
                       2 * sizeof(float));
    ImPlot::EndPlot();
  }
  if (ImPlot::BeginPlot("##Rolling", ImVec2(-1, 150))) {
    ImPlot::SetupAxes(NULL, NULL, flags, flags);
    ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);
    ImPlot::PlotLine("FPS", &rdata1.Data[0].x, &rdata1.Data[0].y,
                     rdata1.Data.size(), 0, 2 * sizeof(float));
    ImPlot::EndPlot();
  }
}

void start_frame() {
  static bool show_demo = false;
  ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
  ImGuiWindowFlags host_window_flags = 0;
  host_window_flags |= ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
  host_window_flags |=
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    host_window_flags |= ImGuiWindowFlags_NoBackground;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);

  if (show_demo) {
    ImGui::ShowDemoWindow(&show_demo);
    ImPlot::ShowDemoWindow();
  }
}

void render_performance_window() {
  ImGui::Begin("Frame Statistics");
  ShowDemo_RealtimePlots();
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::Text("Last CPU frame %.3lf ms", frame_state::last_cpu_frame);
  ImGui::End();
}

void render_general_settings() {
  // show general settings window
  auto &s = input_state::get_input_state();

  ImGui::Begin("General Settings");

  ImGui::SliderFloat3("Camera", glm::value_ptr(s.cam_pos), -100.f, 100.f);

  ImGui::Text(" ");
  ImGui::End();
}

void render_transformation_gui(transformation &t) {
  if (ImGui::SliderFloat3("position", glm::value_ptr(t.translation), -100.f,
                          100.f)) {
  }

  if (ImGui::SliderFloat3("rotation", glm::value_ptr(t.rotation), -180.f,
                          180.f)) {
  }

  if (ImGui::SliderFloat3("scale", glm::value_ptr(t.scale), -100.f, 100.f)) {
  }
}

void render_robot_part_gui(puma::robot_part &rp, const int idx) {
  ImGui::Text("%s%d", "Part #", idx);
  std::string desc = ("Show more##") + std::to_string(idx);
  ImGui::SameLine(100.f);
  if (ImGui::TreeNode(desc.c_str())) {
    ImGui::Checkbox("Visible", &rp.visible);
    ImGui::ColorEdit4("Color", glm::value_ptr(rp.g.color));
    render_transformation_gui(rp.t);
    ImGui::TreePop();
  }
}

void render_robot_gui(puma::robot &r) {
  for (auto i = 0; i < 6; ++i) {
    render_robot_part_gui(r.parts[i], i);
  }
}

void render_scene_gui(puma::scene &s) {
  ImGui::Begin("Scene Settings");

  ImGui::Text("%s", "Light");
  std::string desc = ("Show more##light");
  ImGui::SameLine(160.f);
  if (ImGui::TreeNode(desc.c_str())) {
    ImGui::SliderFloat4("Position", glm::value_ptr(frame_state::light_pos),
                        -10.f, 10.f);
    ImGui::ColorEdit4("Color", glm::value_ptr(frame_state::light_color));
  }

  ImGui::Text("%s", "Robot");
  desc = ("Show more##robot");
  ImGui::SameLine(160.f);
  if (ImGui::TreeNode(desc.c_str())) {
    render_robot_gui(s.r);
    ImGui::TreePop();
  }

  ImGui::Text("%s", "Environment");
  desc = ("Show more##environment");
  ImGui::SameLine(160.f);
  if (ImGui::TreeNode(desc.c_str())) {
    ImGui::TreePop();
  }

  ImGui::Text("%s", "Mirror");
  desc = ("Show more##mirror");
  ImGui::SameLine(160.f);
  if (ImGui::TreeNode(desc.c_str())) {
    ImGui::TreePop();
  }
  ImGui::End();
}

void render_effects_gui() {}

void end_frame() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // update and render additional platform windows
  // (platform functions may change the current opengl context so we
  // save/restore it to make it easier to paste this code elsewhere. For this
  // specific demo appp we could also call glfwMakeCOntextCurrent(window)
  // directly)
  ImGuiIO &io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow *backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

} // namespace gui
