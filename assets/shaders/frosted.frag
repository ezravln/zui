#version 330 core

in vec2 v_position;
in vec2 v_size;
in vec4 v_color;
in float v_radius;

out vec4 frag_color;

uniform vec2 u_resolution;
uniform float u_time;

float hash(vec2 p) {
  return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
  vec2 i = floor(p);
  vec2 f = fract(p);
  f = f * f * (3.0 - 2.0 * f);

  float a = hash(i);
  float b = hash(i + vec2(1.0, 0.0));
  float c = hash(i + vec2(0.0, 1.0));
  float d = hash(i + vec2(1.0, 1.0));

  return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

float fbm(vec2 p) {
  float value = 0.0;
  float amplitude = 0.5;
  for (int i = 0; i < 4; i++) {
    value += amplitude * noise(p);
    p *= 2.0;
    amplitude *= 0.5;
  }
  return value;
}

float rounded_box_sdf(vec2 p, vec2 size, float radius) {
  vec2 q = abs(p) - size + radius;
  return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;
}

void main() {
  vec2 pixel = gl_FragCoord.xy;
  vec2 center = v_position + v_size * 0.5;
  vec2 local = pixel - center;

  float d = rounded_box_sdf(local, v_size * 0.5, v_radius);
  if (d > 0.0) {
    discard;
  }

  vec2 uv = pixel / u_resolution;
  float n = fbm(uv * 80.0);
  n = n * 0.15 + 0.85;

  vec3 base_color = v_color.rgb * n;
  float alpha = v_color.a * smoothstep(1.0, 0.0, d + 1.0);

  frag_color = vec4(base_color, alpha);
}
