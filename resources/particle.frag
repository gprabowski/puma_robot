#version 460

in vec4 color;
in vec3 normal;
in vec3 frag_pos;
out vec4 frag_color;

layout (std140) uniform common_block {
    mat4 proj;
    mat4 view;
    vec4 light_pos;
    vec4 light_color;
    vec4 cam_pos;
};

uniform vec3 intensity;

void main() {
    frag_color = vec4(1, 1, 1, 1);
}
