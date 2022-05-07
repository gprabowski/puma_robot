#pragma once

#include <array>

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
      const std::array<const std::filesystem::path, 6>
          &filenames);
  void recalculate_transformations();
  void draw();
};

struct environment {};
struct mirror {};

struct scene {
  robot r;
  environment e;
  mirror m;
  void draw();
};

}; // namespace puma
