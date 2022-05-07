#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec4 col;

uniform mat4 model;

out vec3 world_pos;

void main() {
    world_pos = vec3(model * vec4(pos, 1));
}
