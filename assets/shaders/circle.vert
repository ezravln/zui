#version 330 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_center;
layout(location = 2) in float a_radius;
layout(location = 3) in vec4 a_color;
layout(location = 4) in float a_thickness;

out vec2 v_local_pos;
out float v_radius;
out vec4 v_color;
out float v_thickness;
out vec2 v_world_pos;

uniform vec2 u_resolution;

void main() {
    vec2 size = vec2(a_radius * 2.0);
    vec2 world_pos = a_center - a_radius + a_pos * size;

    vec2 ndc = (world_pos / u_resolution) * 2.0 - 1.0;
    ndc.y = -ndc.y;

    gl_Position = vec4(ndc, 0.0, 1.0);

    v_local_pos = a_pos * size;
    v_radius = a_radius;
    v_color = a_color;
    v_thickness = a_thickness;
    v_world_pos = world_pos;
}
