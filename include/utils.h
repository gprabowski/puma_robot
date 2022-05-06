#pragma once

#include <gl_object.h>
#include <input_state.h>
#include <memory>
#include <mesh.h>
#include <transformation.h>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace utils {

void set_model_uniform(transformation &t);
void refresh_common_uniforms(GLuint program);

template <typename O> void render_triangles(O &o) {
  transformation &t = o.t;
  gl_object &g = o.g;
  puma::mesh &m = o.m;

  glBindVertexArray(g.vao);
  glUseProgram(g.program);
  utils::set_model_uniform(t);
  glVertexAttrib4f(2, g.color.r, g.color.g, g.color.b, g.color.a);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDrawElements(GL_TRIANGLES, 3 * m.indices.size(), GL_UNSIGNED_INT, NULL);
}

} // namespace utils
