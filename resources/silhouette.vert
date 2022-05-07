#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec4 col;

layout (std140) uniform common_block {
    mat4 proj;
    mat4 view;
    vec4 light_pos;
    vec4 light_color;
    vec4 cam_pos;
};

uniform mat4 model;

out vec3 world_pos;

void main() {
    gl_Position = proj * view * model * vec4(pos, 1.0);
    world_pos = vec3(model * vec4(pos, 1.0));
}
