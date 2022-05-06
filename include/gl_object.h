#pragma once
#include <glad/glad.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>

#include <mesh.h>

struct gl_object {
  GLuint program;
  GLuint vao, vbo, ebo;

  glm::vec4 color{0.2f, 0.2f, 0.2f, 1.0f};

  void reset_api_elements(puma::mesh &m);

  gl_object() {
      glCreateBuffers(1, &vbo);
      glCreateBuffers(1, &ebo);
      glCreateVertexArrays(1, &vao);
  }
  ~gl_object() {
    if (glIsBuffer(vbo)) {
      glDeleteBuffers(1, &vbo);
    }
    if (glIsBuffer(ebo)) {
      glDeleteBuffers(1, &ebo);
    }
    if (glIsVertexArray(vao)) {
      glDeleteVertexArrays(1, &vao);
    }
    if (glIsProgram(program)) {
      // TODO add shader management
      // glDeleteProgram(program);
    }
  }
};
