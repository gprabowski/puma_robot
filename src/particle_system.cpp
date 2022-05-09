#include <particle_system.h>
#include <shader_manager.h>

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
  float random = ((rand() % 100) - 50) / 10.0f;
  float r_color = 0.5f + ((rand() % 100) / 100.0f);
  p.pos = emitter_pos + glm::vec3(random);
  p.color = glm::vec4(r_color, r_color, r_color, 1.0f);
  p.life = 1.0f;
  p.vel = emitter_dir * 0.1f;
}

void puma::particle_system::update() {
  for (unsigned int i = 0; i < this->particles.size(); ++i)
  {
    // subtract from the particles lifetime
    particles[i].life -= dt;

    // if the lifetime is below 0 respawn the particle
    if (particles[i].life <= 0.0f)
    {
      respawn(particles[i]);
    }

    particles[i].pos -= particles[i].vel * dt;
  }
}

void puma::particle_system::init()
{
  emitter_pos = glm::vec3(0, 0, 0);
  emitter_dir = glm::vec3(0, 1, 0);
  // create 100 particles
  particles.resize(100);
  for (unsigned int i = 0; i < particles.size(); ++i)
  {
    respawn(particles[i]);
  }
}