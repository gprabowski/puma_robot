#version 460

in vec2 uv;
in float alpha;

out vec4 frag_color;

uniform vec3 intensity;
uniform sampler2D sparkTexture;

void main() {
    vec4 tex_color = texture(sparkTexture, uv);
    frag_color = vec4(tex_color.xyz, min(tex_color.w, 2*alpha));
}
