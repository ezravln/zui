#version 330 core

in vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_texture;
uniform vec2 u_resolution;
uniform int u_blur_type;      // 0 = gaussian, 1 = zoom
uniform float u_blur_radius;  // blur strength
uniform vec2 u_center;        // center for zoom blur (normalized 0-1)

// Gaussian blur (two-pass optimized single-pass approximation)
vec4 gaussian_blur()
{
    vec2 tex_offset = 1.0 / u_resolution;
    float radius = u_blur_radius;

    vec4 result = vec4(0.0);
    float total_weight = 0.0;

    int samples = int(min(radius * 2.0, 32.0));

    for (int x = -samples; x <= samples; x++) {
        for (int y = -samples; y <= samples; y++) {
            float dist = length(vec2(x, y));
            if (dist > radius) continue;

            float weight = exp(-(dist * dist) / (2.0 * radius * radius));
            vec2 offset = vec2(float(x), float(y)) * tex_offset;
            result += texture(u_texture, v_uv + offset) * weight;
            total_weight += weight;
        }
    }

    return result / total_weight;
}

// Zoom blur (radial blur from center)
vec4 zoom_blur()
{
    vec2 dir = v_uv - u_center;
    float dist = length(dir);

    vec4 result = vec4(0.0);
    int samples = 16;
    float strength = u_blur_radius * 0.01;

    for (int i = 0; i < samples; i++) {
        float t = float(i) / float(samples - 1);
        float scale = 1.0 - strength * t * dist;
        vec2 uv = u_center + dir * scale;
        result += texture(u_texture, uv);
    }

    return result / float(samples);
}

void main()
{
    if (u_blur_radius <= 0.0) {
        frag_color = texture(u_texture, v_uv);
        return;
    }

    if (u_blur_type == 0) {
        frag_color = gaussian_blur();
    } else if (u_blur_type == 1) {
        frag_color = zoom_blur();
    } else {
        frag_color = texture(u_texture, v_uv);
    }
}
