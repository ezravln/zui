#ifndef ZUI_PATH_H
#define ZUI_PATH_H

#include <stdbool.h>
#include <zui/color.h>

typedef struct ZuiPath ZuiPath;

typedef enum {
  ZUI_PATH_MOVE_TO,
  ZUI_PATH_LINE_TO,
  ZUI_PATH_QUAD_TO,
  ZUI_PATH_CUBIC_TO,
  ZUI_PATH_ARC_TO,
  ZUI_PATH_CLOSE,
} ZuiPathCommandType;

typedef struct {
  ZuiPathCommandType type;
  float x, y;
  float cx1, cy1;
  float cx2, cy2;
  float radius;
} ZuiPathCommand;

/**
 * Create a path from an array of commands.
 * The buffer is copied, so the caller can free it after this call.
 */
ZuiPath *zui_path_create(const ZuiPathCommand *commands, int count);

/**
 * Alias for zui_path_create() - creates custom shapes from command buffer.
 */
#define zui_curve_path_create(commands, count) zui_path_create((commands), (count))

/**
 * Create an empty path that can be built incrementally.
 */
ZuiPath *zui_path_new(void);

/**
 * Destroy a path.
 */
void zui_path_destroy(ZuiPath *path);

/**
 * Path building functions.
 */
void zui_path_move_to(ZuiPath *path, float x, float y);
void zui_path_line_to(ZuiPath *path, float x, float y);
void zui_path_quad_to(ZuiPath *path, float cx, float cy, float x, float y);
void zui_path_cubic_to(ZuiPath *path, float cx1, float cy1, float cx2, float cy2, float x, float y);
void zui_path_arc_to(ZuiPath *path, float cx, float cy, float radius, float start_angle, float end_angle);
void zui_path_close(ZuiPath *path);

/**
 * Clear all commands from the path.
 */
void zui_path_clear(ZuiPath *path);

/**
 * Helper functions to create common shapes.
 */
ZuiPath *zui_path_rect(float x, float y, float w, float h);
ZuiPath *zui_path_rounded_rect(float x, float y, float w, float h, float radius);
ZuiPath *zui_path_circle(float cx, float cy, float radius);
ZuiPath *zui_path_ellipse(float cx, float cy, float rx, float ry);
ZuiPath *zui_path_polygon(const float *points, int point_count);
ZuiPath *zui_path_regular_polygon(float cx, float cy, float radius, int sides);
ZuiPath *zui_path_star(float cx, float cy, float outer_radius, float inner_radius, int points);
ZuiPath *zui_path_rounded_polygon(const float *points, int point_count, float radius);

/**
 * Transform functions.
 */
void zui_path_translate(ZuiPath *path, float dx, float dy);
void zui_path_scale(ZuiPath *path, float sx, float sy);
void zui_path_rotate(ZuiPath *path, float angle, float cx, float cy);

/**
 * Get path bounds.
 */
void zui_path_get_bounds(ZuiPath *path, float *x, float *y, float *w, float *h);

/**
 * Check if a point is inside the path.
 */
bool zui_path_contains(ZuiPath *path, float x, float y);

/**
 * Get the raw path commands for rendering.
 */
const ZuiPathCommand *zui_path_get_commands(ZuiPath *path, int *count);

/**
 * Drawing functions.
 * Call these between zui_begin_frame() and zui_end_frame().
 */
void zui_draw_path(ZuiPath *path, ZuiColor fill_color);
void zui_draw_path_stroke(ZuiPath *path, ZuiColor stroke_color, float thickness);

#endif
