#version 330

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 18) out;

layout (std140) uniform common_block {
    mat4 proj;
    mat4 view;
    vec4 light_pos;
    vec4 light_color;
    vec4 cam_pos;
};

uniform mat4 model;

in vec3 world_pos[];

float EPSILON = 0.01;

// emit a quad using a triangle strip
void emit_quad(vec3 start_vertex, vec3 end_vertex)
{
    // vertex #1: the starting vertex (just a tiny bit below the original edge)
    vec3 light_dir = normalize(start_vertex - light_pos.xyz);
    gl_Position = proj * view * vec4((start_vertex + light_dir * EPSILON), 1.0);
    EmitVertex();

    // vertex #2: the starting vertex projected to infinity
    gl_Position = proj * view * vec4(light_dir, 0.0);
    EmitVertex();

    // vertex #3: the ending vertex (just a tiny bit below the original edge)
    light_dir = normalize(end_vertex - light_pos.xyz);
    gl_Position = proj * view * vec4((end_vertex + light_dir * EPSILON), 1.0);
    EmitVertex();

    // vertex #4: the ending vertex projected to infinity
    gl_Position = proj * view * vec4(light_dir, 0.0);
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

    if (dot(normal, light_dir) > 0) {

        normal = cross(e3,e1);

        if (dot(normal, light_dir) <= 0) {
            vec3 start = world_pos[0];
            vec3 end = world_pos[2];
            emit_quad(start, end);
        }

        normal = cross(e4,e5);
        light_dir = light_pos.xyz - world_pos[2];

        if (dot(normal, light_dir) <= 0) {
            vec3 start = world_pos[2];
            vec3 end = world_pos[4];
            emit_quad(start, end);
        }

        normal = cross(e2,e6);
        light_dir = light_pos.xyz - world_pos[4];

        if (dot(normal, light_dir) <= 0) {
            vec3 start = world_pos[4];
            vec3 end = world_pos[0];
            emit_quad(start, end);
        }

        // render the front cap
        light_dir = (normalize(world_pos[0] - light_pos.xyz));
        gl_Position = proj * view * vec4((world_pos[0] + light_dir * EPSILON), 1.0);
        EmitVertex();

        light_dir = (normalize(world_pos[2] - light_pos.xyz));
        gl_Position = proj * view * vec4((world_pos[2] + light_dir * EPSILON), 1.0);
        EmitVertex();

        light_dir = (normalize(world_pos[4] - light_pos.xyz));
        gl_Position = proj * view * vec4((world_pos[4] + light_dir * EPSILON), 1.0);
        EmitVertex();

        EndPrimitive();
 
        // render the back cap
        light_dir = world_pos[0] - light_pos.xyz;
        gl_Position = proj * view * vec4(light_dir, 0.0);
        EmitVertex();

        light_dir = world_pos[4] - light_pos.xyz;
        gl_Position = proj * view * vec4(light_dir, 0.0);
        EmitVertex();

        light_dir = world_pos[2] - light_pos.xyz;
        gl_Position = proj * view * vec4(light_dir, 0.0);
        EmitVertex();

        EndPrimitive();
    }
}