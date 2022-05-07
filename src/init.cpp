#include "glad/glad.h"

#include <cstdio>
#include <cstdlib>

#include <callbacks.h>
#include <frame_state.h>
#include <gui.h>
#include <init.h>
#include <input_state.h>
#include <log.h>
#include <shader_manager.h>

namespace clb = callbacks;

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  glViewport(0, 0, width, height);
}

void glfw_window_destroyer(GLFWwindow *w) {
  GK2_PUMA_INFO("Destroying GLFW window");
  glfwDestroyWindow(w);
}

static void glfw_error_callback(int error, const char *description) {
  GK2_PUMA_ERROR("{0}: {1}", error, description);
}

void teardown() { glfwTerminate(); }

void glfw_die(const char *message) {
  const char *err;
  glfwGetError(&err);
  GK2_PUMA_CRITICAL("{0} : {1} ", message, err);
  exit(-1);
}

static void APIENTRY openglCallbackFunction(GLenum source, GLenum type,
                                            GLuint id, GLenum severity,
                                            GLsizei length,
                                            const GLchar *message,
                                            const void *userParam) {
  (void)source;
  (void)type;
  (void)id;
  (void)severity;
  (void)length;
  (void)userParam;
  GK2_PUMA_ERROR("{0}", message);
  if (severity == GL_DEBUG_SEVERITY_HIGH) {
    GK2_PUMA_CRITICAL("Aborting...");
    abort();
  }
}

namespace init {

void glfw_window_hints() {
  glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_SAMPLES, 8);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

#ifndef RELEASE_MODE
  // Debugging
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
}

void glfw_setup() {
  glfwSetErrorCallback(glfw_error_callback);
  atexit(teardown);

  if (!glfwInit()) {
    glfw_die("Couldn't initialize GLFW");
  }

  GK2_PUMA_INFO("GLFW Successfully initialized");
}

std::shared_ptr<GLFWwindow> glfw_get_window(const int w, const int h,
                                            const char *title) {
  auto window = std::shared_ptr<GLFWwindow>(
      glfwCreateWindow(w, h, title, NULL, NULL), glfw_window_destroyer);

  if (!window) {
    glfw_die("Couldn't create a GLFW window");
  }
  return window;
}

void glfw_set_window_options(std::shared_ptr<GLFWwindow> &w) {
  glfwMakeContextCurrent(w.get());
  glfwSetFramebufferSizeCallback(w.get(), framebuffer_size_callback);
  glfwSwapInterval(1);
}

void glad_setup() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfw_die("[ERROR] Couldn't initialize GLAD");
  } else {
    GK2_PUMA_INFO("GLAD Successfully initialized");
  }
}

void ogl_print_info() {
  GK2_PUMA_INFO("OpenGL loaded");
  GK2_PUMA_INFO("Vendor: {0}",
                reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
  GK2_PUMA_INFO("Renderer: {0}",
                reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
  GK2_PUMA_INFO("Version: {0}",
                reinterpret_cast<const char *>(glGetString(GL_VERSION)));
}

void ogl_setup(std::shared_ptr<GLFWwindow> w) {
#ifndef RELEASE_MODE
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(openglCallbackFunction, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                        true);
#endif

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

  int actualWindowWidth, actualWindowHeight;
  glfwGetWindowSize(w.get(), &actualWindowWidth, &actualWindowHeight);
  glViewport(0, 0, actualWindowWidth, actualWindowHeight);
  glClearColor(0.1f, 0.3f, 0.2f, 1.0f);
}

std::shared_ptr<GLFWwindow> init_all(const char *caption) {
  static constexpr int init_w = 1600;
  static constexpr int init_h = 900;

  // GLFW
  glfw_setup();
  glfw_window_hints();
  auto w = glfw_get_window(init_w, init_h, caption);
  glfw_set_window_options(w);

  // GLAD
  glad_setup();

  // OPENGL STATE
  ogl_print_info();
  ogl_setup(w);

  clb::set_keyboard_callback(w);
  clb::set_mouse_callback(w);

  auto &sm = shader_manager::get_manager();
  // read in all shaders
  sm.add(shader_t::DEFAULT_SHADER, "resources/general");

  gui::setup_gui(w);

  frame_state::freq = glfwGetTimerFrequency();

  frame_state::light_pos = {1.0, 4.0, 2.0, 0.0};
  frame_state::light_color = {1.0, 1.0, 1.0, 1.0};

  return w;
}

void cleanup() { gui::cleanup_gui(); }
} // namespace init
