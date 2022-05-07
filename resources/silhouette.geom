#version 330

layout (triangles_adjacency) in;
layout (line_strip, max_vertices = 6) out;

layout (std140) uniform common_block {
    mat4 proj;
    mat4 view;
    vec4 light_pos;
    vec4 light_color;
    vec4 cam_pos;
};

in vec3 world_pos[];

void emit_line(int start_index, int end_index)
{
    gl_Position = gl_in[start_index].gl_Position;
    EmitVertex();

    gl_Position = gl_in[end_index].gl_Position;
    EmitVertex();

    EndPrimitive();
}

void main()
{
    vec3 e1 = world_pos[2] - world_pos[0];
    vec3 e2 = world_pos[4] - world_pos[0];
    vec3 e3 = world_pos[1] - world_pos[0];
    vec3 e4 = world_pos[3] - world_pos[2];
    vec3 e5 = world_pos[4] - world_pos[2];
    vec3 e6 = world_pos[5] - world_pos[0];

    vec3 normal = cross(e1,e2);
    vec3 light_dir = light_pos.xyz - world_pos[0];

    if (dot(normal, light_dir) > 0.00001) {

        normal = cross(e3,e1);

        if (dot(normal, light_dir) <= 0) {
            emit_line(0, 2);
        }

        normal = cross(e4,e5);
        light_dir = light_pos.xyz - world_pos[2];

        if (dot(normal, light_dir) <=0) {
            emit_line(2, 4);
        }

        normal = cross(e2,e6);
        light_dir = light_pos.xyz - world_pos[4];

        if (dot(normal, light_dir) <= 0) {
            emit_line(4, 0);
        }
    }
}