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

  void load_parts_from_files(
      const std::array<const std::filesystem::path::value_type *, 6>
          &filenames);
  void draw();
};

struct environment {};
struct mirror {};

struct scene {
  robot r;
  environment e;
  mirror m;
  void draw();
  static scene get_initialized_scene() {
    scene s;
    s.r.load_parts_from_files({"assets/mesh1.txt", "assets/mesh2.txt",
                               "assets/mesh3.txt", "assets/mesh4.txt",
                               "assets/mesh5.txt", "assets/mesh6.txt"});
    return s;
  }
};

}; // namespace puma
