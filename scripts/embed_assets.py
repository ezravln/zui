#!/usr/bin/env python3
"""
Generate C headers from asset files for embedding into the binary.
Usage: python3 embed_assets.py
"""

import os
import sys

ASSETS_DIR = os.path.join(os.path.dirname(__file__), '..', 'assets')
OUTPUT_DIR = os.path.join(os.path.dirname(__file__), '..', 'src', 'embedded')

ICONS_TO_EMBED = [
    ('icons/x-exit-icon.svg', 'exit_icon'),
    ('icons/x-minimize-icon.svg', 'minimize_icon'),
    ('icons/x-maximize-icon.svg', 'maximize_icon'),
    ('icons/x-hidden-icon.svg', 'hidden_icon'),
    ('icons/x-check-icon.svg', 'check_icon'),
    ('icons/x-chevron-up.svg', 'chevron_up_icon'),
    ('icons/x-chevron-down.svg', 'chevron_down_icon'),
    ('logo/zui-logo-white.png', 'logo_white'),
]

SHADERS_TO_EMBED = [
    ('shaders/rect.vert', 'shader_rect_vert'),
    ('shaders/rect.frag', 'shader_rect_frag'),
    ('shaders/texture.vert', 'shader_texture_vert'),
    ('shaders/texture.frag', 'shader_texture_frag'),
    ('shaders/glyph.vert', 'shader_glyph_vert'),
    ('shaders/glyph.frag', 'shader_glyph_frag'),
    ('shaders/circle.vert', 'shader_circle_vert'),
    ('shaders/circle.frag', 'shader_circle_frag'),
    ('shaders/arc.vert', 'shader_arc_vert'),
    ('shaders/arc.frag', 'shader_arc_frag'),
]

ASSETS_TO_EMBED = ICONS_TO_EMBED + SHADERS_TO_EMBED

def file_to_c_array(filepath, var_name):
    with open(filepath, 'rb') as f:
        data = f.read()

    lines = []
    lines.append(f'static const unsigned char {var_name}_data[] = {{')

    # Format as hex bytes, 16 per line
    hex_values = [f'0x{b:02x}' for b in data]
    for i in range(0, len(hex_values), 16):
        chunk = ', '.join(hex_values[i:i+16])
        lines.append(f'  {chunk},')

    lines.append('};')
    lines.append(f'static const unsigned int {var_name}_size = {len(data)};')

    return '\n'.join(lines)

def generate_header(asset_path, var_name):
    filepath = os.path.join(ASSETS_DIR, asset_path)
    if not os.path.exists(filepath):
        print(f'Warning: {filepath} not found, skipping')
        return None

    guard_name = f'ZUI_EMBEDDED_{var_name.upper()}_H'

    content = f'''#ifndef {guard_name}
#define {guard_name}

{file_to_c_array(filepath, var_name)}

#endif
'''
    return content

def generate_main_header():
    lines = [
        '#ifndef ZUI_EMBEDDED_ASSETS_H',
        '#define ZUI_EMBEDDED_ASSETS_H',
        '',
    ]

    for asset_path, var_name in ASSETS_TO_EMBED:
        lines.append(f'#include "embedded_{var_name}.h"')

    lines.append('')
    lines.append('#endif')
    lines.append('')

    return '\n'.join(lines)

def generate_shaders_header():
    lines = [
        '#ifndef ZUI_EMBEDDED_SHADERS_H',
        '#define ZUI_EMBEDDED_SHADERS_H',
        '',
        '#include <stddef.h>',
        '#include <string.h>',
        '',
    ]

    # Include individual shader headers
    for asset_path, var_name in SHADERS_TO_EMBED:
        lines.append(f'#include "embedded_{var_name}.h"')

    lines.append('')
    lines.append('typedef struct {')
    lines.append('  const char *name;')
    lines.append('  const char *data;')
    lines.append('  unsigned int size;')
    lines.append('} EmbeddedShader;')
    lines.append('')
    lines.append('static const EmbeddedShader embedded_shaders[] = {')

    for asset_path, var_name in SHADERS_TO_EMBED:
        # Extract filename from path (e.g., "shaders/rect.vert" -> "rect.vert")
        filename = os.path.basename(asset_path)
        lines.append(f'  {{"{filename}", (const char *){var_name}_data, {var_name}_size}},')

    lines.append('  {NULL, NULL, 0}')
    lines.append('};')
    lines.append('')
    lines.append('static inline const char *zui_get_embedded_shader(const char *name) {')
    lines.append('  for (int i = 0; embedded_shaders[i].name != NULL; i++) {')
    lines.append('    if (strcmp(name, embedded_shaders[i].name) == 0) {')
    lines.append('      return embedded_shaders[i].data;')
    lines.append('    }')
    lines.append('  }')
    lines.append('  return NULL;')
    lines.append('}')
    lines.append('')
    lines.append('#endif')
    lines.append('')

    return '\n'.join(lines)

def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    for asset_path, var_name in ASSETS_TO_EMBED:
        content = generate_header(asset_path, var_name)
        if content:
            output_path = os.path.join(OUTPUT_DIR, f'embedded_{var_name}.h')
            with open(output_path, 'w') as f:
                f.write(content)
            print(f'Generated: {output_path}')

    # Generate main include header
    main_header = generate_main_header()
    main_header_path = os.path.join(OUTPUT_DIR, 'embedded_assets.h')
    with open(main_header_path, 'w') as f:
        f.write(main_header)
    print(f'Generated: {main_header_path}')

    # Generate shaders header
    shaders_header = generate_shaders_header()
    shaders_header_path = os.path.join(OUTPUT_DIR, 'embedded_shaders.h')
    with open(shaders_header_path, 'w') as f:
        f.write(shaders_header)
    print(f'Generated: {shaders_header_path}')

    print('Done!')

if __name__ == '__main__':
    main()
