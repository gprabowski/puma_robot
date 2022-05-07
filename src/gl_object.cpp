#include <gl_object.h>
#include <mesh.h>
#include <transformation.h>

void gl_object::reset_api_elements(puma::mesh &m) {
  // allocation or reallocation
  glNamedBufferData(vbo, sizeof(puma::vertex_t) * m.vertices.size(),
                    m.vertices.data(), GL_STATIC_DRAW);

  // allocation or reallocation
  glNamedBufferData(ebo, sizeof(m.adjacent_tris[0]) * m.adjacent_tris.size(),
                    m.adjacent_tris.data(), GL_STATIC_DRAW);

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
