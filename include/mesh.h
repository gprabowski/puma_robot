#pragma once
#include <array>
#include <filesystem>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace puma {

struct vertex_t {
  glm::vec3 pos;
  glm::vec3 norm;
};

struct triangle_t {
  std::array<GLuint, 3> indices;
};

struct mesh {
  std::vector<glm::vec3> unique_positions;
  std::vector<vertex_t> vertices;
  std::vector<triangle_t> indices;
};

} // namespace puma
