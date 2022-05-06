#pragma once

#include <gl_object.h>
#include <input_state.h>
#include <memory>
#include <transformation.h>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace utils {

void render_gl(const gl_object &g);

void render_app();

void refresh_common_uniforms(GLuint program);

void reset_gl_objects(gl_object &g);
void set_model_uniform(const transformation &t);

} // namespace utils
