#include <cstdlib>
#include <istream>
#include <set>

#include <glad/glad.h>

#define GLM_FORCE_RADIANS
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <frame_state.h>
#include <gl_object.h>
#include <log.h>
#include <utils.h>

#include <mtxlib.h>

namespace utils {

void inverse_kinematics(vector3 pos, vector3 normal, float &a1, float &a2,
                        float &a3, float &a4, float &a5) {
  float l1 = .91f, l2 = .81f, l3 = .33f, dy = .27f, dz = .26f;
  normal.normalize();
  vector3 pos1 = pos + normal * l3;
  float e = sqrtf(pos1.z * pos1.z + pos1.x * pos1.x - dz * dz);
  a1 = atan2(pos1.z, -pos1.x) + atan2(dz, e);
  vector3 pos2(e, pos1.y - dy, .0f);
  a3 = -acosf(
      fmin(1.0f, (pos2.x * pos2.x + pos2.y * pos2.y - l1 * l1 - l2 * l2) /
                     (2.0f * l1 * l2)));
  float k = l1 + l2 * cosf(a3), l = l2 * sinf(a3);
  a2 = -atan2(pos2.y, sqrtf(pos2.x * pos2.x + pos2.z * pos2.z)) - atan2(l, k);
  vector3 normal1;
  normal1 = vector3(RotateRadMatrix44('y', -a1) *
                    vector4(normal.x, normal.y, normal.z, .0f));
  normal1 = vector3(RotateRadMatrix44('z', -(a2 + a3)) *
                    vector4(normal1.x, normal1.y, normal1.z, .0f));
  a5 = acosf(normal1.x);
  a4 = atan2(normal1.z, normal1.y);
}

void get_model_uniform(transformation &t, glm::mat4 &out) {
  GLint program;
  glGetIntegerv(GL_CURRENT_PROGRAM, &program);

  const auto trans = glm::translate(glm::mat4(1.0f), t.translation);
  const auto scale = glm::scale(glm::mat4(1.0f), t.scale);
  const auto rot = glm::toMat4(glm::quat(glm::radians(t.rotation)));

  const auto model = trans * scale * rot;

  out = model;
}

void set_model_uniform(transformation &t) {
  GLint program;
  glGetIntegerv(GL_CURRENT_PROGRAM, &program);

  const auto trans = glm::translate(glm::mat4(1.0f), t.translation);
  const auto scale = glm::scale(glm::mat4(1.0f), t.scale);
  const auto rot = glm::toMat4(glm::quat(glm::radians(t.rotation)));

  const auto model = trans * scale * rot;

  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE,
                     glm::value_ptr(model));
}

void set_lighting_uniforms(GLfloat ambient, GLfloat diffuse, GLfloat specular) {
  GLint program;
  glGetIntegerv(GL_CURRENT_PROGRAM, &program);
  glUniform3f(glGetUniformLocation(program, "intensity"), ambient, diffuse, specular);
}

} // namespace utils
