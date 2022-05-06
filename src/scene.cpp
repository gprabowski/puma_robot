#include <scene.h>
#include <utils.h>

void puma::robot::load_parts_from_files(
    const std::array<const std::filesystem::path::value_type *, 6> &filenames) {

}

void puma::robot::draw() {
  for (auto &p : parts) {
    utils::render_triangles(p);
  }
}
