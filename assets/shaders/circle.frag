#version 330 core

in vec2 v_local_pos;
in float v_radius;
in vec4 v_color;
in float v_thickness;
in vec2 v_world_pos;

uniform vec4 u_clip_rect;
uniform float u_clip_radius;
uniform bool u_clip_enabled;

out vec4 frag_color;

float rounded_box_sdf(vec2 p, vec2 b, vec4 r) {
    float rx = (p.x > 0.0) ? ((p.y > 0.0) ? r.z : r.y) : ((p.y > 0.0) ? r.w : r.x);
    rx = min(rx, min(b.x, b.y));
    vec2 q = abs(p) - b + rx;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - rx;
}

void main() {
    vec2 center = vec2(v_radius);
    float dist = length(v_local_pos - center);

    float alpha;
    if (v_thickness > 0.0) {
        float outer = v_radius;
        float inner = v_radius - v_thickness;
        alpha = smoothstep(outer + 0.5, outer - 0.5, dist) *
                smoothstep(inner - 0.5, inner + 0.5, dist);
    } else {
        alpha = smoothstep(v_radius + 0.5, v_radius - 0.5, dist);
    }

    if (u_clip_enabled) {
        vec2 clip_half = u_clip_rect.zw * 0.5;
        vec2 clip_center = u_clip_rect.xy + clip_half;
        vec2 clip_p = v_world_pos - clip_center;
        float clip_d = rounded_box_sdf(clip_p, clip_half - 0.5,
            vec4(u_clip_radius, u_clip_radius, u_clip_radius, u_clip_radius));
        float clip_alpha = 1.0 - smoothstep(-0.5, 0.5, clip_d);
        alpha *= clip_alpha;
    }

    alpha *= v_color.a;
    frag_color = vec4(v_color.rgb * alpha, alpha);
}
