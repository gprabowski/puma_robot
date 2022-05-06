#include <scene.h>

void puma::robot_part::draw() {
  glUseProgram(g.program);
  glVertexAttrib4f(1, g.color.r, g.color.g, g.color.b, g.color.a);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDrawElements(GL_TRIANGLES, 3 * g.m.indices.size(), GL_UNSIGNED_INT, NULL);
}
