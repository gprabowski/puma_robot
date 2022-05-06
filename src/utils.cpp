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

namespace utils {

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

} // namespace utils
