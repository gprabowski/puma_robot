#pragma once
#include <array>
#include <filesystem>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <cassert>

namespace puma {

struct vertex_t {
  glm::vec3 pos;
  glm::vec3 norm;
};

struct edge_t {
  GLuint v1, v2;

  edge_t(GLuint _v1, GLuint _v2) {
    assert(_v1 != _v2);
    if (_v1 < _v2) {
      v1 = _v1;
      v2 = _v2;
    } else {
      v1 = _v2;
      v2 = _v1;      
    }
  }
};

struct neighbors_t {
	GLuint t1, t2;

  GLuint other(GLuint t) {
    assert(t == t1 || t == t2);
    return t == t1 ? t2 : t1;
  }
};

struct compare_edges
{
  bool operator()(const edge_t& e1, const edge_t& e2) const
  {
    if (e1.v1 < e2.v1) {
      return true;
    }
    else if (e1.v1 == e2.v1) {
      return (e1.v2 < e2.v2);
    }
    else {
      return false;
    }
  }
};

struct triangle_t {
  std::array<GLuint, 3> indices;
};

struct mesh {
  std::vector<glm::vec3> unique_positions;
  std::vector<vertex_t> vertices;
  std::vector<triangle_t> tris;
  std::vector<GLuint> adjacent_tris;
  std::vector<GLuint> vertex_index;
  std::map<edge_t, neighbors_t, compare_edges> edges;

  GLuint opposite_vertex(const edge_t& edge, const triangle_t& triangle) const {
    for (GLuint i = 0; i < 3; i++) {
      GLuint index = vertex_index[triangle.indices[i]];
      if (index != edge.v1 && index != edge.v2) {
        return triangle.indices[i];
      }
    }
    assert(0);
    return 0;
  }
};

} // namespace puma
