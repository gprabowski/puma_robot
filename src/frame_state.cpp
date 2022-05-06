#include <frame_state.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

glm::mat4 frame_state::proj;
glm::mat4 frame_state::view;

glm::vec4 frame_state::light_pos;
glm::vec4 frame_state::light_color;

int frame_state::window_w;
int frame_state::window_h;

ImVec2 frame_state::content_area;
ImVec2 frame_state::content_pos;

uint64_t frame_state::freq = glfwGetTimerFrequency();
double frame_state::last_cpu_frame;
