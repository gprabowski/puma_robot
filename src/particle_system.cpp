#include <particle_system.h>
#include <shader_manager.h>

#include <mesh.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h";

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
  float life = 0.1f + ((rand() % 50) / 100.0f);
  p.pos = emitter_pos;
  p.life = life;
  p.vel = (-emitter_dir + glm::vec3(vx, vy, vz)) * 0.6f;
}

void puma::particle_system::update() {
  int emitted = 0;

  for (unsigned int i = 0; i < particles.size(); ++i)
  {
    // subtract from the particles lifetime
    particles[i].life -= dt;

    particles[i].pos -= particles[i].vel * dt;
    particles[i].vel.y += 9.81 * dt;

    m.vertices[2 * i].pos = m.vertices[2 * i + 1].pos;
    m.vertices[2 * i + 1].pos = particles[i].pos;
    // ugly and hacky but allows us to use the same layout
    m.vertices[2 * i].norm = glm::vec3(particles[i].life, 0, 0);
    m.vertices[2 * i + 1].norm = glm::vec3(particles[i].life, 0, 0);
  }

  for (unsigned int i = 0; i < this->particles.size(); ++i)
  {
    // if the lifetime is below 0 respawn the particle
    if (particles[i].life <= 0.0f)
    {
      respawn(particles[i]);
      m.vertices[2 * i].pos = particles[i].pos;
      m.vertices[2 * i + 1].pos = particles[i].pos;
      emitted++;
      if (emitted > 3)
        break;
    }
  }
  
  g.reset_api_elements(m);
}

void puma::particle_system::init()
{
  // generate particle texture
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  int width, height, nrChannels;
  unsigned char* data = stbi_load("assets/spark.png", &width, &height, &nrChannels, 0);
  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  stbi_image_free(data);

  emitter_pos = glm::vec3(2, 0, 0);
  emitter_dir = glm::vec3(1, 0, 0);
  // create 100 particles
  particles.resize(100);
  for (unsigned int i = 0; i < particles.size(); ++i)
  {
    // add corresponding mesh vertices (last and current position)
    m.vertices.emplace_back(vertex_t());
    m.vertices.emplace_back(vertex_t());
    m.elements.emplace_back(2*i);
    m.elements.emplace_back(2*i + 1);
  }
  t.translation = glm::vec3(0, 0, 0);
  t.rotation = glm::vec3(0, 0, 0);
  t.scale = glm::vec3(1, 1, 1);
}