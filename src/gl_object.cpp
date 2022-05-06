#include <gl_object.h>
#include <mesh.h>
#include <transformation.h>

void gl_object::reset_api_elements(puma::mesh &m) {
  if (!glIsBuffer(vbo)) {
    glCreateBuffers(1, &vbo);
  }
  // allocation or reallocation
  glNamedBufferData(vbo, sizeof(puma::vertex_t) * m.vertices.size(),
                    m.vertices.data(), GL_STATIC_DRAW);

  if (!glIsBuffer(ebo)) {
    glCreateBuffers(1, &ebo);
  }
  // allocation or reallocation
  glNamedBufferData(ebo, sizeof(puma::triangle_t) * m.indices.size(),
                    m.indices.data(), GL_STATIC_DRAW);

  if (!glIsVertexArray(vao)) {
    glCreateVertexArrays(1, &vao);
  }

  glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(puma::vertex_t));
  glVertexArrayElementBuffer(vao, ebo);

  glEnableVertexArrayAttrib(vao, 0);
  glEnableVertexArrayAttrib(vao, 1);

  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE,
                            offsetof(puma::vertex_t, pos));
  glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE,
                            offsetof(puma::vertex_t, norm));

  glVertexArrayAttribBinding(vao, 0, 0);
  glVertexArrayAttribBinding(vao, 1, 0);
}

void gl_object::load(const std::filesystem::path::value_type *shader_file) {}

void gl_object::draw() {}
