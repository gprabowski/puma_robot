#include <log.h>
#include <scene.h>
#include <utils.h>

#include <framebuffer.h>
#include <fstream>
#include <iostream>
#include <shader_manager.h>
#include <sstream>
#include <string>
#include <vector>

#include <frame_state.h>
#include <frame_update.h>

std::vector<std::string> read_mesh_file(const std::filesystem::path mesh_file) {
  std::vector<std::string> ret;
  std::ifstream ifs;

  auto ex = ifs.exceptions();
  ex |= std::ios_base::badbit | std::ios_base::failbit;
  // ifs.exceptions(ex);

  ifs.open(mesh_file);
  ifs.ignore(std::numeric_limits<std::streamsize>::max());
  // auto size = ifs.gcount();
  // GK2_PUMA_INFO("[MESH] Read {0} bytes from {1}", size, mesh_file);

  ifs.clear();
  ifs.seekg(0, std::ios_base::beg);

  std::string current;
  while (getline(ifs, current)) {
    ret.push_back(current);
  }

  return ret;
}

void puma::robot::load_parts_from_files(
    const std::array<const std::filesystem::path, 6> &filenames) {
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
      p.m.vertex_index.emplace_back(idx);
      p.m.vertices.push_back(
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
      p.m.tris.emplace_back(triangle_t{a, b, c});
      ss.clear();
    }

    // read silhouette edges into a map
    std::size_t num_edges;
    ss << lines[curr_idx++];
    ss >> num_edges;
    ss.clear();
    for (std::size_t e = 0u; e < num_edges; ++e) {
      unsigned int v1, v2, t1, t2;
      ss << lines[curr_idx++];
      ss >> v1 >> v2 >> t1 >> t2;
      auto edge = puma::edge_t(v1, v2);
      p.m.edges[edge] = neighbors_t{t1, t2};
      ss.clear();
    }

    // generate adjacenct tris indices for silhouette rendering
    for (std::size_t t = 0u; t < num_triangles; t++) {
      const puma::triangle_t &triangle = p.m.tris[t];

      for (std::size_t v = 0u; v < 3; v++) {
        auto v1 = p.m.vertex_index[triangle.indices[v]];
        auto v2 = p.m.vertex_index[triangle.indices[(v + 1) % 3]];
        puma::edge_t e(v1, v2);
        assert(p.m.edges.find(e) != p.m.edges.end());
        puma::neighbors_t n = p.m.edges[e];

        const puma::triangle_t &other_triangle = p.m.tris[n.other(t)];

        GLuint opposite = p.m.opposite_vertex(e, other_triangle);

        p.m.elements.push_back(triangle.indices[v]);
        p.m.elements.push_back(opposite);
      }
    }

    axes = {glm::vec3{0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}};

    positions = {glm::vec3{0.0f, 0.0f, 0.0f},
                 {0.0f, 0.27f, 0.0f},
                 {-0.91f, 0.27f, 0.0f},
                 {0.0f, 0.27f, -0.26f},
                 {-1.72f, 0.27f, 0.0f}};

    for (std::size_t i = 0; i < angles.size(); ++i) {
      angles[i] = 0.0f;
      axes[i] = glm::normalize(axes[i]);
    }
    p.g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
    p.g.reset_api_elements(p.m);
  }
}

void decompose(const glm::mat4 &m, glm::vec3 &trans, glm::vec3 &scale,
               glm::vec3 &rot) {
  trans = glm::vec3(m[3]);
  scale = {glm::length(glm::vec3(m[0])), glm::length(glm::vec3(m[1])),
           glm::length(glm::vec3(m[2]))};

  glm::mat4 m_rot(m[0] / scale.x, m[1] / scale.y, m[2] / scale.z,
                  glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  rot = glm::degrees(glm::eulerAngles(glm::quat_cast(m_rot)));
}

void puma::robot::recalculate_transformations() {
  glm::mat4 current_transform = glm::mat4(1.0f);
  glm::vec3 dummy;
  for (int i = 1; i < 6; ++i) {
    auto t1 = glm::translate(glm::mat4(1.0f), -1.f * positions[i - 1]);
    auto r1 = glm::rotate(glm::mat4(1.0f), angles[i - 1], axes[i - 1]);
    auto t2 = glm::translate(glm::mat4(1.0f), positions[i - 1]);
    current_transform = current_transform * t2 * r1 * t1;
    decompose(current_transform, parts[i].t.translation, dummy,
              parts[i].t.rotation);
  }
}

void puma::scene::render_into_depth() {
  auto &sm = shader_manager::get_manager();

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  for (auto &p : r.parts) {
    p.g.program = sm.programs[shader_t::NULL_SHADER].idx;
    utils::render_triangles(p, GL_TRIANGLES_ADJACENCY);
  }
  e.g.program = sm.programs[shader_t::NULL_SHADER].idx;
  utils::render_triangles(e, GL_TRIANGLES);
}

void puma::scene::render_into_stencil() {
  auto &sm = shader_manager::get_manager();

  glDepthMask(GL_FALSE);
  glEnable(GL_DEPTH_CLAMP);
  glDisable(GL_CULL_FACE);

  // we need the stencil test to be enabled but we want it
  // to succeed always. only the depth test matters.
  glStencilFunc(GL_ALWAYS, 0, 0x0f);

  // set the stencil test per the depth fail algorithm
  glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
  glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

  for (auto &p : r.parts) {
    p.g.program = sm.programs[shader_t::SHADOW_VOLUME_SHADER].idx;
    utils::render_triangles(p, GL_TRIANGLES_ADJACENCY);
  }

  // mirror
  glStencilFunc(GL_ALWAYS, 0xa0, 0xf0);

  glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_REPLACE);
  glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_REPLACE);

  m.g.program = sm.programs[shader_t::NULL_SHADER].idx;
  utils::render_triangles(m, GL_TRIANGLES);

  glDepthMask(GL_TRUE);
  glDisable(GL_DEPTH_CLAMP);
  glEnable(GL_CULL_FACE);
}

void puma::scene::render_mirror() {
  auto &sm = shader_manager::get_manager();
  auto &is = input_state::get_input_state();

  if (glm::dot(m.t.translation - is.cam_pos, m.current_normal) <= 0) {
    glCullFace(GL_FRONT);
    glStencilFunc(GL_EQUAL, 0xa0, 0xf0);
    glDepthFunc(GL_ALWAYS);
    glm::mat4 transform;
    utils::get_model_uniform(m.t, transform);

    auto mirror_mtx = transform *
                      glm::scale(glm::mat4(1.0f), glm::vec3{-1, 1, 1}) *
                      glm::inverse(transform);
    auto old_lp = frame_state::light_pos;
    frame_state::light_pos = mirror_mtx * glm::vec4(glm::vec3(old_lp), 1.0f);

    update::refresh_view(frame_state::view * mirror_mtx);

    e.g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
    e.g.intensity = {0.2, 0.8, 0.8};
    utils::render_triangles(e, GL_TRIANGLES);

    glDepthFunc(GL_LEQUAL);
    for (auto &p : r.parts) {
      p.g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
      p.g.intensity = {0.2, 0.8, 0.8};
      utils::render_triangles(p, GL_TRIANGLES_ADJACENCY);
    }

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m.g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
    m.g.intensity = {0.2, 0.8, 0.8};
    auto oc = m.g.color;
    m.g.color = {0.5f, 0.0f, 0.0f, 0.2f};
    utils::render_triangles(m, GL_TRIANGLES);
    m.g.color = oc;

    frame_state::light_pos = old_lp;
    glCullFace(GL_BACK);
  } else {
    m.g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
    m.g.intensity = {0.2, 0.8, 0.8};
    utils::render_triangles(m, GL_TRIANGLES);
  }
}
void puma::scene::render_shadowed() {
  auto &sm = shader_manager::get_manager();

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  // prevent update to the stencil buffer
  glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
  // Setup mirror matrix
  // draw only if the corresponding stencil value is zero
  glStencilFunc(GL_EQUAL, 0x0, 0x0f);

  glDepthFunc(GL_LEQUAL);

  auto no_ambient = glm::vec3(0.0f, 0.8f, 0.8f);

  for (auto &p : r.parts) {
    p.g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
    p.g.intensity = no_ambient;
    utils::render_triangles(p, GL_TRIANGLES_ADJACENCY);
  }
  e.g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
  e.g.intensity = no_ambient;
  utils::render_triangles(e, GL_TRIANGLES);
}

void puma::scene::render_ambient() {
  auto &sm = shader_manager::get_manager();

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);
  glStencilFunc(GL_NOTEQUAL, 0xa0, 0xf0);

  auto ambient = glm::vec3(0.2f, 0.0f, 0.0f);

  for (auto &p : r.parts) {
    p.g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
    p.g.intensity = ambient;
    utils::render_triangles(p, GL_TRIANGLES_ADJACENCY);
  }
  e.g.intensity = ambient;
  e.g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
  utils::render_triangles(e, GL_TRIANGLES);

  glDisable(GL_BLEND);
}

void puma::scene::draw() {
  update::setup_globals();
  update::refresh_ubos();
  glDepthMask(GL_TRUE);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  render_into_depth();

  glEnable(GL_STENCIL_TEST);

  render_into_stencil();

  render_shadowed();

  render_mirror();
  update::refresh_ubos();

  render_ambient();

  glDisable(GL_STENCIL_TEST);
}

void puma::environment::generate() {
  static auto &sm = shader_manager::get_manager();
  // generate vertices
  m.vertices = {
      // east wall
      {{5.0f, 0.0f, -5.0f}, {-1.0f, 0.0f, 0.0f}},
      {{5.0f, 5.0f, -5.0f}, {-1.0f, 0.0f, 0.0f}},
      {{5.0f, 5.0f, 5.0f}, {-1.0f, 0.0f, 0.0f}},
      {{5.0f, 0.0f, 5.0f}, {-1.0f, 0.0f, 0.0f}},

      // west wall
      {{-5.0f, 0.0f, -5.0f}, {1.0f, 0.0f, 0.0f}},
      {{-5.0f, 5.0f, -5.0f}, {1.0f, 0.0f, 0.0f}},
      {{-5.0f, 5.0f, 5.0f}, {1.0f, 0.0f, 0.0f}},
      {{-5.0f, 0.0f, 5.0f}, {1.0f, 0.0f, 0.0f}},

      // north wall
      {{-5.0f, 0.0f, 5.0f}, {0.0f, 0.0f, -1.0f}},
      {{-5.0f, 5.0f, 5.0f}, {0.0f, 0.0f, -1.0f}},
      {{5.0f, 5.0f, 5.0f}, {0.0f, 0.0f, -1.0f}},
      {{5.0f, 0.0f, 5.0f}, {0.0f, 0.0f, -1.0f}},

      // south wall
      {{-5.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 1.0f}},
      {{-5.0f, 5.0f, -5.0f}, {0.0f, 0.0f, 1.0f}},
      {{5.0f, 5.0f, -5.0f}, {0.0f, 0.0f, 1.0f}},
      {{5.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 1.0f}},

      // floor
      {{-5.0f, 0.0f, 5.0f}, {0.0f, 1.0f, 0.0f}},
      {{-5.0f, 0.0f, -5.0f}, {0.0f, 1.0f, 0.0f}},
      {{5.0f, 0.0f, -5.0f}, {0.0f, 1.0f, 0.0f}},
      {{5.0f, 0.0f, 5.0f}, {0.0f, 1.0f, 0.0f}},

      // ceiling
      {{-5.0f, 5.0f, 5.0f}, {0.0f, -1.0f, 0.0f}},
      {{-5.0f, 5.0f, -5.0f}, {0.0f, -1.0f, 0.0f}},
      {{5.0f, 5.0f, -5.0f}, {0.0f, -1.0f, 0.0f}},
      {{5.0f, 5.0f, 5.0f}, {0.0f, -1.0f, 0.0f}},
  };

  // generate indices
  m.elements = {
      0,  2,  1,  2,  0,  3,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
      12, 14, 13, 15, 14, 12, 16, 18, 17, 18, 16, 19, 20, 21, 22, 22, 23, 20,
  };

  t.rotation = {0, 0, 0};
  t.translation = {0.0f, -1.0f, 0.0f};
  g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
  g.reset_api_elements(m);
}

void puma::mirror::generate() {
  static auto &sm = shader_manager::get_manager();
  // generate vertices
  m.vertices = {
      {{-0.0f, -1.0f, 0.7f}, {1.0f, 0.0f, 0.0f}},
      {{-0.0f, -1.0f, -0.7f}, {1.0f, 0.0f, 0.0f}},
      {{-0.0f, 1.0f, -0.7f}, {1.0f, 0.0f, 0.0f}},
      {{-0.0f, 1.0f, 0.7f}, {1.0f, 0.0f, 0.0f}},

      {{-0.0f, -1.0f, 0.7f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.0f, -1.0f, -0.7f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.0f, 1.0f, -0.7f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.0f, 1.0f, 0.7f}, {-1.0f, 0.0f, 0.0f}},
  };

  constexpr float initial_angle = 0;
  // generate indices
  m.tris = {{0, 1, 2}, {2, 3, 0}, {5, 4, 6}, {4, 7, 6}};
  m.elements = {0, 1, 2, 2, 3, 0, 5, 4, 6, 4, 7, 6};
  t.rotation = {0, 0, initial_angle};
  t.translation = {-1.80, 0.0f, -0.2};
  // get gl
  g.program = sm.programs[shader_t::DEFAULT_SHADER].idx;
  current_normal = glm::rotate(glm::mat4(1.0f), glm::radians(initial_angle),
                               {0.0f, 0.0f, 1.0f}) *
                   glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  g.reset_api_elements(m);
}

void puma::mirror::move(double delta) {
  static float total_time = 0.0f;
  total_time += delta * speed;
  constexpr float r = 0.3f;
  // get new point location
  current_point =
      r * glm::vec3(0.0f, glm::sin(total_time), glm::cos(total_time));

  current_point =
      glm::translate(glm::mat4(1.0f),
                     {t.translation.x, t.translation.y, t.translation.z}) *
      glm::rotate(glm::mat4(1.0f), glm::radians(t.rotation.z),
                  {0.0, 0.0, 1.0f}) *
      glm::vec4(current_point, 1.0f);

  current_normal = glm::rotate(glm::mat4(1.0f), glm::radians(t.rotation.z),
                               {0.0f, 0.0f, 1.0f}) *
                   glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
}
