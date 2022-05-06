#include <log.h>
#include <scene.h>
#include <utils.h>

#include <fstream>
#include <shader_manager.h>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string>
read_mesh_file(const std::filesystem::path::value_type *mesh_file) {
  std::vector<std::string> ret;
  std::ifstream ifs;

  auto ex = ifs.exceptions();
  ex |= std::ios_base::badbit | std::ios_base::failbit;
  // ifs.exceptions(ex);

  ifs.open(mesh_file);
  ifs.ignore(std::numeric_limits<std::streamsize>::max());
  auto size = ifs.gcount();
  GK2_PUMA_INFO("[MESH] Read {0} bytes from {1}", size, mesh_file);

  ifs.clear();
  ifs.seekg(0, std::ios_base::beg);

  std::string current;
  while (getline(ifs, current)) {
    ret.push_back(current);
  }

  return ret;
}

void puma::robot::load_parts_from_files(
    const std::array<const std::filesystem::path::value_type *, 6> &filenames) {
  auto &sm = shader_manager::get_manager();
  std::stringstream ss;
  for (std::size_t i = 0u; i < parts.size(); ++i) {
    auto &p = parts[i];
    auto lines = read_mesh_file(filenames[i]);

    std::size_t curr_idx{0u};

    // read unique positions
    std::size_t num_unique_positions{0u};
    ss << lines[curr_idx++];
    ss >> num_unique_positions;
    ss.clear();
    for (std::size_t up = 0u; up < num_unique_positions; ++up) {
      float x, y, z;
      ss << lines[curr_idx++];
      ss >> x >> y >> z;
      p.m.unique_positions.emplace_back(glm::vec3{x, y, z});
      ss.clear();
    }

    // read normals
    std::size_t num_vertices{0u};
    ss << lines[curr_idx++];
    ss >> num_vertices;
    ss.clear();
    for (std::size_t v = 0u; v < num_vertices; ++v) {
      std::size_t idx;
      float x, y, z;
      ss << lines[curr_idx++];
      ss >> idx >> x >> y >> z;
      p.m.vertices.emplace_back(
          vertex_t{p.m.unique_positions[idx], glm::vec3{x, y, z}});
      ss.clear();
    }
    // read triangle indices
    std::size_t num_triangles{0u};
    ss << lines[curr_idx++];
    ss >> num_triangles;
    ss.clear();
    for (std::size_t t = 0u; t < num_triangles; ++t) {
      unsigned int a, b, c;
      ss << lines[curr_idx++];
      ss >> a >> b >> c;
      p.m.indices.emplace_back(triangle_t{a, b, c});
      ss.clear();
    }

    p.g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
    p.g.reset_api_elements(p.m);
  }
}

void puma::robot::draw() {
  for (auto &p : parts) {
    utils::render_triangles(p);
  }
}

void puma::scene::draw() { r.draw(); }
