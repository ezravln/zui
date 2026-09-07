#version 330 core

in vec2 v_local_pos;
in float v_radius;
in vec4 v_color;
in float v_start_angle;
in float v_end_angle;
in float v_thickness;
in vec2 v_world_pos;

uniform vec4 u_clip_rect;
uniform float u_clip_radius;
uniform bool u_clip_enabled;

out vec4 frag_color;

#define PI 3.14159265359

float rounded_box_sdf(vec2 p, vec2 b, vec4 r) {
    float rx = (p.x > 0.0) ? ((p.y > 0.0) ? r.z : r.y) : ((p.y > 0.0) ? r.w : r.x);
    rx = min(rx, min(b.x, b.y));
    vec2 q = abs(p) - b + rx;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - rx;
}

float angle_in_range(float angle, float start, float end, float dist) {
    angle = mod(angle, 2.0 * PI);
    if (angle < 0.0) angle += 2.0 * PI;

    float s = mod(start, 2.0 * PI);
    if (s < 0.0) s += 2.0 * PI;

    float e = mod(end, 2.0 * PI);
    if (e < 0.0) e += 2.0 * PI;

    float edge_width = 1.5 / max(dist, 1.0);

    // Calculate signed angular distance (positive = inside, negative = outside)
    float signed_dist;

    if (s <= e) {
        // Non-wrapping case
        if (angle < s) {
            signed_dist = angle - s;
        } else if (angle > e) {
            signed_dist = e - angle;
        } else {
            signed_dist = min(angle - s, e - angle);
        }
    } else {
        // Wrapping case (e.g., start=350°, end=10°)
        if (angle >= s) {
            signed_dist = min(angle - s, 2.0 * PI - angle + e);
        } else if (angle <= e) {
            signed_dist = min(e - angle, 2.0 * PI - s + angle);
        } else {
            // In the gap
            float to_start = s - angle;
            float to_end = angle - e;
            signed_dist = -min(to_start, to_end);
        }
    }

    return smoothstep(-edge_width * 0.5, edge_width * 0.5, signed_dist);
}

void main() {
    vec2 center = vec2(v_radius);
    vec2 p = v_local_pos - center;
    float dist = length(p);

    float angle = atan(p.y, p.x);
    if (angle < 0.0) angle += 2.0 * PI;

    float in_arc = angle_in_range(angle, v_start_angle, v_end_angle, dist);

    float alpha;
    if (v_thickness > 0.0) {
        float outer = v_radius;
        float inner = v_radius - v_thickness;
        alpha = smoothstep(outer + 0.5, outer - 0.5, dist) *
                smoothstep(inner - 0.5, inner + 0.5, dist);
    } else {
        alpha = smoothstep(v_radius + 0.5, v_radius - 0.5, dist);
    }

    alpha *= in_arc;

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
