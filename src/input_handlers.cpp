#include <input_handlers.h>

#include <frame_state.h>
#include <shader_manager.h>
#include <utils.h>

namespace handlers {

void handle_keyboard() {
  static float delta_time = 0.0f;
  static float last_frame = 0.0f;

  auto &state = input_state::get_input_state();

  float currentFrame = glfwGetTime();
  delta_time = currentFrame - last_frame;
  last_frame = currentFrame;

  const float cameraSpeed = 30.f * delta_time; // adjust accordingly
  if (state.pressed[GLFW_KEY_W]) {
    state.cam_pos += cameraSpeed * state.cam_front;
  }
  if (state.pressed[GLFW_KEY_S]) {
    state.cam_pos -= cameraSpeed * state.cam_front;
  }
  if (state.pressed[GLFW_KEY_A]) {
    state.cam_pos -=
        glm::normalize(glm::cross(state.cam_front, state.cam_up)) * cameraSpeed;
  }
  if (state.pressed[GLFW_KEY_D]) {
    state.cam_pos +=
        glm::normalize(glm::cross(state.cam_front, state.cam_up)) * cameraSpeed;
  }
}

inline bool intersect(const glm::vec3 &ray_orig, const glm::vec3 &dir,
                      const glm::vec3 &sphere_center,
                      const float sphere_radius) {
  const auto L = sphere_center - ray_orig;
  const auto tc = glm::dot(L, dir);
  if (tc <= 0.0) {
    return false;
  }
  const float d2 = glm::dot(L, L) - tc * tc;
  const float radius2 = sphere_radius * sphere_radius;

  return d2 <= radius2;
}

void handle_mouse() {
  auto &state = input_state::get_input_state();

  if (state.mouse_just_pressed[input_state::mouse_button::left] &&
      state.pressed[GLFW_KEY_LEFT_CONTROL]) {

    state.mouse_just_pressed.reset(input_state::mouse_button::left);

    const auto ndc_x = 2 * (((state.last_mouse.x - frame_state::content_pos.x) /
                             static_cast<float>(frame_state::content_area.x)) -
                            0.5f);
    const auto ndc_y =
        (-2) * ((state.last_mouse.y - frame_state::content_pos.y) /
                    static_cast<float>(frame_state::content_area.y) -
                0.5f);

    glm::vec4 ndc_dir{ndc_x, ndc_y, -1, 1};

    glm::vec4 world_p = glm::inverse(frame_state::proj) * ndc_dir;
    world_p = world_p / world_p.z;
    world_p = glm::inverse(frame_state::view) * world_p;
    world_p = world_p / world_p.w;

    // auto dir = glm::normalize(glm::vec3(world_p) - state.cam_pos);
  }
}

void process_input() {
  using fs = frame_state;
  if (!ImGui::IsMouseHoveringRect(fs::content_pos,
                                  {fs::content_pos.x + fs::content_area.x,
                                   fs::content_pos.y + fs::content_area.y})) {
    return;
  }
  handle_keyboard();
  handle_mouse();
}

} // namespace handlers
