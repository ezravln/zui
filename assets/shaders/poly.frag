#version 330 core

uniform vec4 u_color;
uniform bool u_clip_enabled;
uniform vec4 u_clip_rect;
uniform float u_clip_radius;

out vec4 frag_color;

float rounded_box_sdf(vec2 p, vec2 size, float radius)
{
  vec2 q = abs(p) - size + radius;
  return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;
}

void main()
{
  if (u_clip_enabled) {
    vec2 clip_center = u_clip_rect.xy + u_clip_rect.zw * 0.5;
    vec2 clip_size = u_clip_rect.zw * 0.5;
    float d = rounded_box_sdf(gl_FragCoord.xy - clip_center, clip_size, u_clip_radius);
    if (d > 0.0) discard;
  }

  frag_color = u_color;
}
