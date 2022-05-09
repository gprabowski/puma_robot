#pragma once

#define GLM_FORCE_RADIANS
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <transformation.h>
#include <gl_object.h>
#include <mesh.h>

namespace puma {
  struct particle {
    glm::vec3 pos, vel;
    float     life;

    particle() : pos(0.0f), vel(0.0f), life(0.0f) {}
  };

  struct particle_system {
    unsigned int n_particles;
    unsigned int last_used = 0;
    unsigned int texture;
    float dt = 0.01f;
    std::vector<particle> particles;
    glm::vec3 emitter_pos;
    glm::vec3 emitter_dir;
    gl_object g;
    transformation t;
    mesh m;
    bool visible{ true };

    void init();
    unsigned int first_unused();
    void respawn(particle &p);
    void update();
  };
}