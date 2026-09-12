#version 330 core

layout(location = 0) in vec2 a_vertex;
layout(location = 1) in vec2 a_position;
layout(location = 2) in vec2 a_size;
layout(location = 3) in vec4 a_color;
layout(location = 4) in float a_radius;

out vec2 v_position;
out vec2 v_size;
out vec4 v_color;
out float v_radius;

uniform mat4 u_projection;

void main() {
  vec2 pos = a_position + a_vertex * a_size;
  gl_Position = u_projection * vec4(pos, 0.0, 1.0);

  v_position = a_position;
  v_size = a_size;
  v_color = a_color;
  v_radius = a_radius;
}
