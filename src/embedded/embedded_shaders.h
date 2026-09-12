#ifndef ZUI_EMBEDDED_SHADERS_H
#define ZUI_EMBEDDED_SHADERS_H

#include <stddef.h>
#include <string.h>

#include "embedded_shader_rect_vert.h"
#include "embedded_shader_rect_frag.h"
#include "embedded_shader_texture_vert.h"
#include "embedded_shader_texture_frag.h"
#include "embedded_shader_glyph_vert.h"
#include "embedded_shader_glyph_frag.h"
#include "embedded_shader_circle_vert.h"
#include "embedded_shader_circle_frag.h"
#include "embedded_shader_arc_vert.h"
#include "embedded_shader_arc_frag.h"
#include "embedded_shader_poly_vert.h"
#include "embedded_shader_poly_frag.h"

typedef struct {
  const char *name;
  const char *data;
  unsigned int size;
} EmbeddedShader;

static const EmbeddedShader embedded_shaders[] = {
  {"rect.vert", (const char *)shader_rect_vert_data, shader_rect_vert_size},
  {"rect.frag", (const char *)shader_rect_frag_data, shader_rect_frag_size},
  {"texture.vert", (const char *)shader_texture_vert_data, shader_texture_vert_size},
  {"texture.frag", (const char *)shader_texture_frag_data, shader_texture_frag_size},
  {"glyph.vert", (const char *)shader_glyph_vert_data, shader_glyph_vert_size},
  {"glyph.frag", (const char *)shader_glyph_frag_data, shader_glyph_frag_size},
  {"circle.vert", (const char *)shader_circle_vert_data, shader_circle_vert_size},
  {"circle.frag", (const char *)shader_circle_frag_data, shader_circle_frag_size},
  {"arc.vert", (const char *)shader_arc_vert_data, shader_arc_vert_size},
  {"arc.frag", (const char *)shader_arc_frag_data, shader_arc_frag_size},
  {"poly.vert", (const char *)shader_poly_vert_data, shader_poly_vert_data_len},
  {"poly.frag", (const char *)shader_poly_frag_data, shader_poly_frag_data_len},
  {NULL, NULL, 0}
};

static inline const char *zui_get_embedded_shader(const char *name) {
  for (int i = 0; embedded_shaders[i].name != NULL; i++) {
    if (strcmp(name, embedded_shaders[i].name) == 0) {
      return embedded_shaders[i].data;
    }
  }
  return NULL;
}

#endif
