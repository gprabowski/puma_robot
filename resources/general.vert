#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec4 col;

layout (std140) uniform common_block {
    mat4 proj;
    mat4 view;
};

uniform mat4 model;

out vec4 color;

void main() {
    gl_Position = proj * view * model * vec4(pos, 1.0);
    color = col;
}
