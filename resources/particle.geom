#version 150

layout (lines) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;    
   
layout (std140) uniform common_block {
    mat4 proj;
    mat4 view;
    vec4 light_pos;
    vec4 light_color;
    vec4 cam_pos;
};

in float life[];

out float alpha;
out vec2 uv;
   
void main (void)
{
  vec3 p1 = gl_in[0].gl_Position.xyz;
  vec3 p2 = gl_in[1].gl_Position.xyz;

  vec3 right = -normalize((p2 - p1));

  vec3 cam_dir = normalize(p1 - cam_pos.xyz);

  vec3 up = normalize(cross(cam_dir, right));
  
  vec3 va = p1 - (right + up) * 0.02f;
  gl_Position = proj * view * vec4(va, 1.0);
  uv = vec2(0.0, 0.0);
  alpha = life[0];
  EmitVertex();  
  
  vec3 vb = p1 - (right - up) * 0.02f;
  gl_Position = proj * view * vec4(vb, 1.0);
  uv = vec2(0.0, 1.0);
  alpha = life[0];
  EmitVertex();  

  vec3 vd = p1 + (right - up) * 0.02f;
  gl_Position = proj * view * vec4(vd, 1.0);
  uv = vec2(1.0, 0.0);
  alpha = life[0];
  EmitVertex();  

  vec3 vc = p1 + (right + up) * 0.02f;
  gl_Position = proj * view * vec4(vc, 1.0);
  uv = vec2(1.0, 1.0);
  alpha = life[0];
  EmitVertex();  
  
  EndPrimitive();  
}   