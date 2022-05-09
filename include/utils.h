#pragma once

#include <gl_object.h>
#include <input_state.h>
#include <memory>
#include <mesh.h>
#include <transformation.h>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <mtxlib.h>

namespace utils {

  void set_model_uniform(transformation& t);
  void set_lighting_uniforms(GLfloat ambient, GLfloat diffuse, GLfloat specular);
  void get_model_uniform(transformation& t, glm::mat4& out);
  void refresh_common_uniforms(GLuint program);

  void inverse_kinematics(vector3 pos, vector3 normal, float& a1, float& a2,
    float& a3, float& a4, float& a5);

  template <typename O> void render_primitives(O& o, GLenum primitives) {
    if (!o.visible) {
      return;
    }
    transformation& t = o.t;
    gl_object& g = o.g;
    puma::mesh& m = o.m;

    glBindVertexArray(g.vao);
    glUseProgram(g.program);
    utils::set_model_uniform(t);
    utils::set_lighting_uniforms(g.intensity[0], g.intensity[1], g.intensity[2]);
    glVertexAttrib4f(2, g.color.r, g.color.g, g.color.b, g.color.a);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(primitives, m.elements.size(), GL_UNSIGNED_INT, NULL);
  }

}// namespace utils
