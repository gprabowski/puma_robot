#pragma once

#include <gl_object.h>
#include <transformation.h>

namespace puma {

struct robot_part {
  gl_object g;
  transformation t;
  mesh m;

  // don't judge me
  bool visible{true};
  void draw();
};

struct robot {};
struct environment {};
struct mirror {};

struct scene {
  robot r;
  environment e;
  mirror m;
  static scene get_initialized_scene() {
    scene s;
    return s;
  }
};

}; // namespace puma
