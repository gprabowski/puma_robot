#pragma once

#include <array>

#define GLM_FORCE_RADIANS
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <gl_object.h>
#include <transformation.h>

namespace puma {

struct robot_part {
  gl_object g;
  transformation t;
  mesh m;

  // don't judge me
  bool visible{true};
};

struct robot {
  std::array<robot_part, 6> parts;
  std::array<float, 5> angles;
  std::array<glm::vec3, 5> axes;
  std::array<glm::vec3, 5> positions;

  void load_parts_from_files(
      const std::array<const std::filesystem::path::value_type *, 6>
          &filenames);
  void recalculate_transformations();
  void draw();
};

struct environment {};

struct mirror {
  gl_object g;
  transformation t;
  mesh m;
  bool visible{true};
  float speed{1.0f};
  glm::vec3 current_point{0.0f, 0.0f, 0.0f};
  glm::vec3 current_normal;

  void generate();
  void move(double delta);
};

struct scene {
  robot r;
  environment e;
  mirror m;
  bool animation{true};
  void draw();
  scene() {
    r.load_parts_from_files({"assets/mesh1.txt", "assets/mesh2.txt",
                             "assets/mesh3.txt", "assets/mesh4.txt",
                             "assets/mesh5.txt", "assets/mesh6.txt"});
    m.generate();
  }
};

}; // namespace puma
