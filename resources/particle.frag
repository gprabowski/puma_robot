#version 460

in vec2 uv;
in float alpha;

out vec4 frag_color;

uniform vec3 intensity;

void main() {
    frag_color = vec4(1, 1, 1, alpha);
}
