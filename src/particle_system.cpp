#include <particle_system.h>
#include <shader_manager.h>

#include <mesh.h>

unsigned int puma::particle_system::first_unused() {
  for (unsigned int i = last_used; i < n_particles; ++i) {
    if (particles[i].life <= 0.0f) {
      last_used = i;
      return i;
    }
  }
  // otherwise, do a linear search
  for (unsigned int i = 0; i < last_used; ++i) {
    if (particles[i].life <= 0.0f) {
      last_used = i;
      return i;
    }
  }
  // override first particle if all others are alive
  last_used = 0;
  return 0;
}

void puma::particle_system::respawn(particle &p) {
  float vx = ((rand() % 100) - 50) / 30.0f;
  float vy = ((rand() % 100) - 50) / 30.0f;
  float vz = ((rand() % 100) - 50) / 30.0f;
  float life = 0.5f + ((rand() % 50) / 100.0f);
  p.pos = emitter_pos;
  p.life = life;
  p.vel = (-emitter_dir + glm::vec3(vx, vy, vz)) * 0.6f;
}

void puma::particle_system::update() {
  int emitted = 0;

  for (unsigned int i = 0; i < this->particles.size(); ++i)
  {
    // if the lifetime is below 0 respawn the particle
    if (particles[i].life <= 0.0f)
    {
      respawn(particles[i]);
      emitted++;
      if (emitted > 1)
        break;
    }
  }

  for (unsigned int i = 0; i < this->particles.size(); ++i)
  {
    // subtract from the particles lifetime
    particles[i].life -= dt;

    particles[i].pos -= particles[i].vel * dt;
    particles[i].vel.y += 9.81 * dt;

    m.vertices[i].pos = particles[i].pos;
    // hacky but allows us to use the same layout
    m.vertices[i].norm = particles[i].vel;
  }
  g.reset_api_elements(m);
}

void puma::particle_system::init()
{
  emitter_pos = glm::vec3(2, 0, 0);
  emitter_dir = glm::vec3(1, 0, 0);
  // create 100 particles
  particles.resize(200);
  for (unsigned int i = 0; i < particles.size(); ++i)
  {
    // add corresponding mesh vertex
    m.vertices.emplace_back(vertex_t());
    m.elements.emplace_back(i);
  }
  t.translation = glm::vec3(0, 0, 0);
  t.rotation = glm::vec3(0, 0, 0);
  t.scale = glm::vec3(1, 1, 1);
}