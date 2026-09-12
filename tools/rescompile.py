#!/usr/bin/env python3
"""
ZUI Resource Compiler

Reads a .res file and generates C code with embedded binary data.

.res file format:
  # Comment
  res_path = file_path

Example:
  images/logo.png = assets/logo.png
  fonts/main.ttf = assets/fonts/roboto.ttf

Or shorthand (resource path = file path):
  assets/logo.png
"""

import sys
import os
import argparse

def read_res_file(res_path, base_dir):
    """Parse .res file and return list of (res_path, file_path) tuples."""
    resources = []

    with open(res_path, 'r') as f:
        for line_num, line in enumerate(f, 1):
            line = line.strip()

            # Skip empty lines and comments
            if not line or line.startswith('#'):
                continue

            if '=' in line:
                # Explicit mapping: res_path = file_path
                parts = line.split('=', 1)
                res_name = parts[0].strip()
                file_path = parts[1].strip()
            else:
                # Shorthand: file path is resource path
                res_name = line
                file_path = line

            # Make file path absolute relative to base_dir
            if not os.path.isabs(file_path):
                file_path = os.path.join(base_dir, file_path)

            if not os.path.exists(file_path):
                print(f"Warning: Resource file not found: {file_path} (line {line_num})", file=sys.stderr)
                continue

            resources.append((res_name, file_path))

    return resources

def generate_c_code(resources, output_name, prefix="zui"):
    """Generate C source and header files for embedded resources."""

    guard_name = output_name.upper().replace('-', '_').replace('.', '_')
    func_prefix = prefix

    header = f"""#ifndef {guard_name}_H
#define {guard_name}_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {{
    const char *path;
    const unsigned char *data;
    size_t size;
}} {func_prefix.capitalize()}EmbeddedResource;

const {func_prefix.capitalize()}EmbeddedResource *{func_prefix}_resource_find(const char *path);
const unsigned char *{func_prefix}_resource_get(const char *path, size_t *size);
bool {func_prefix}_resource_exists(const char *path);

#endif
"""

    source_parts = [
        f'#include "{output_name}.h"',
        '#include <string.h>',
        '',
    ]

    # Generate data arrays for each resource
    for i, (res_path, file_path) in enumerate(resources):
        with open(file_path, 'rb') as f:
            data = f.read()

        # Create C array
        var_name = f"res_data_{i}"
        source_parts.append(f"/* {res_path} */")
        source_parts.append(f"static const unsigned char {var_name}[] = {{")

        # Format bytes in rows of 16
        for j in range(0, len(data), 16):
            chunk = data[j:j+16]
            hex_values = ', '.join(f'0x{b:02x}' for b in chunk)
            source_parts.append(f"    {hex_values},")

        source_parts.append("};")
        source_parts.append(f"#define res_size_{i} {len(data)}")
        source_parts.append("")

    # Generate resource table
    source_parts.append(f"static const {func_prefix.capitalize()}EmbeddedResource g_{func_prefix}_resources[] = {{")
    for i, (res_path, _) in enumerate(resources):
        source_parts.append(f'    {{ "{res_path}", res_data_{i}, res_size_{i} }},')
    source_parts.append("    { NULL, NULL, 0 }")
    source_parts.append("};")
    source_parts.append("")

    # Generate lookup functions
    source_parts.append(f"""
const {func_prefix.capitalize()}EmbeddedResource *{func_prefix}_resource_find(const char *path)
{{
    if (!path) return NULL;

    /* Skip res:/ prefix if present */
    if (strncmp(path, "res:/", 5) == 0) {{
        path += 5;
    }} else if (strncmp(path, "res:", 4) == 0) {{
        path += 4;
    }}

    /* Skip leading slash */
    while (*path == '/') path++;

    for (int i = 0; g_{func_prefix}_resources[i].path != NULL; i++) {{
        if (strcmp(g_{func_prefix}_resources[i].path, path) == 0) {{
            return &g_{func_prefix}_resources[i];
        }}
    }}
    return NULL;
}}

const unsigned char *{func_prefix}_resource_get(const char *path, size_t *size)
{{
    const {func_prefix.capitalize()}EmbeddedResource *res = {func_prefix}_resource_find(path);
    if (!res) {{
        if (size) *size = 0;
        return NULL;
    }}
    if (size) *size = res->size;
    return res->data;
}}

bool {func_prefix}_resource_exists(const char *path)
{{
    return {func_prefix}_resource_find(path) != NULL;
}}
""")

    source = '\n'.join(source_parts)

    return header, source

def main():
    parser = argparse.ArgumentParser(description='ZUI Resource Compiler')
    parser.add_argument('input', help='Input .res file')
    parser.add_argument('-o', '--output', default='resources_generated',
                        help='Output base name (default: resources_generated)')
    parser.add_argument('-d', '--output-dir', default='.',
                        help='Output directory')
    parser.add_argument('-p', '--prefix', default='zui',
                        help='Function prefix (default: zui)')

    args = parser.parse_args()

    if not os.path.exists(args.input):
        print(f"Error: Input file not found: {args.input}", file=sys.stderr)
        return 1

    base_dir = os.path.dirname(os.path.abspath(args.input))
    resources = read_res_file(args.input, base_dir)

    if not resources:
        print("Warning: No resources found", file=sys.stderr)

    print(f"Compiling {len(resources)} resources...")

    header, source = generate_c_code(resources, args.output, args.prefix)

    os.makedirs(args.output_dir, exist_ok=True)

    header_path = os.path.join(args.output_dir, f"{args.output}.h")
    source_path = os.path.join(args.output_dir, f"{args.output}.c")

    with open(header_path, 'w') as f:
        f.write(header)

    with open(source_path, 'w') as f:
        f.write(source)

    print(f"Generated: {header_path}")
    print(f"Generated: {source_path}")

    return 0

if __name__ == '__main__':
    sys.exit(main())
