#include <zui/path.h>
#include <zui/internal/renderer.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

extern ZuiRenderer *zui_get_renderer(void);

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define PATH_INITIAL_CAPACITY 16

struct ZuiPath {
  ZuiPathCommand *commands;
  int count;
  int capacity;
  float bounds_x, bounds_y, bounds_w, bounds_h;
  bool bounds_dirty;
};

static void ensure_capacity(ZuiPath *path, int needed)
{
  if (path->count + needed <= path->capacity) return;

  int new_capacity = path->capacity * 2;
  if (new_capacity < path->count + needed) {
    new_capacity = path->count + needed;
  }

  ZuiPathCommand *new_commands = realloc(path->commands,
    (size_t)new_capacity * sizeof(ZuiPathCommand));
  if (new_commands) {
    path->commands = new_commands;
    path->capacity = new_capacity;
  }
}

static void add_command(ZuiPath *path, ZuiPathCommand cmd)
{
  ensure_capacity(path, 1);
  path->commands[path->count++] = cmd;
  path->bounds_dirty = true;
}

ZuiPath *zui_path_create(const ZuiPathCommand *commands, int count)
{
  ZuiPath *path = calloc(1, sizeof(ZuiPath));
  if (!path) return NULL;

  path->capacity = count > PATH_INITIAL_CAPACITY ? count : PATH_INITIAL_CAPACITY;
  path->commands = malloc((size_t)path->capacity * sizeof(ZuiPathCommand));
  if (!path->commands) {
    free(path);
    return NULL;
  }

  if (commands && count > 0) {
    memcpy(path->commands, commands, (size_t)count * sizeof(ZuiPathCommand));
    path->count = count;
  }

  path->bounds_dirty = true;
  return path;
}

ZuiPath *zui_path_new(void)
{
  return zui_path_create(NULL, 0);
}

void zui_path_destroy(ZuiPath *path)
{
  if (!path) return;
  free(path->commands);
  free(path);
}

void zui_path_move_to(ZuiPath *path, float x, float y)
{
  if (!path) return;
  ZuiPathCommand cmd = {
    .type = ZUI_PATH_MOVE_TO,
    .x = x,
    .y = y,
  };
  add_command(path, cmd);
}

void zui_path_line_to(ZuiPath *path, float x, float y)
{
  if (!path) return;
  ZuiPathCommand cmd = {
    .type = ZUI_PATH_LINE_TO,
    .x = x,
    .y = y,
  };
  add_command(path, cmd);
}

void zui_path_quad_to(ZuiPath *path, float cx, float cy, float x, float y)
{
  if (!path) return;
  ZuiPathCommand cmd = {
    .type = ZUI_PATH_QUAD_TO,
    .x = x,
    .y = y,
    .cx1 = cx,
    .cy1 = cy,
  };
  add_command(path, cmd);
}

void zui_path_cubic_to(ZuiPath *path, float cx1, float cy1, float cx2, float cy2, float x, float y)
{
  if (!path) return;
  ZuiPathCommand cmd = {
    .type = ZUI_PATH_CUBIC_TO,
    .x = x,
    .y = y,
    .cx1 = cx1,
    .cy1 = cy1,
    .cx2 = cx2,
    .cy2 = cy2,
  };
  add_command(path, cmd);
}

void zui_path_arc_to(ZuiPath *path, float cx, float cy, float radius, float start_angle, float end_angle)
{
  if (!path) return;
  ZuiPathCommand cmd = {
    .type = ZUI_PATH_ARC_TO,
    .x = cx,
    .y = cy,
    .radius = radius,
    .cx1 = start_angle,
    .cy1 = end_angle,
  };
  add_command(path, cmd);
}

void zui_path_close(ZuiPath *path)
{
  if (!path) return;
  ZuiPathCommand cmd = {
    .type = ZUI_PATH_CLOSE,
  };
  add_command(path, cmd);
}

void zui_path_clear(ZuiPath *path)
{
  if (!path) return;
  path->count = 0;
  path->bounds_dirty = true;
}

ZuiPath *zui_path_rect(float x, float y, float w, float h)
{
  ZuiPath *path = zui_path_new();
  if (!path) return NULL;

  zui_path_move_to(path, x, y);
  zui_path_line_to(path, x + w, y);
  zui_path_line_to(path, x + w, y + h);
  zui_path_line_to(path, x, y + h);
  zui_path_close(path);

  return path;
}

ZuiPath *zui_path_rounded_rect(float x, float y, float w, float h, float r)
{
  ZuiPath *path = zui_path_new();
  if (!path) return NULL;

  if (r > w / 2) r = w / 2;
  if (r > h / 2) r = h / 2;

  zui_path_move_to(path, x + r, y);
  zui_path_line_to(path, x + w - r, y);
  zui_path_quad_to(path, x + w, y, x + w, y + r);
  zui_path_line_to(path, x + w, y + h - r);
  zui_path_quad_to(path, x + w, y + h, x + w - r, y + h);
  zui_path_line_to(path, x + r, y + h);
  zui_path_quad_to(path, x, y + h, x, y + h - r);
  zui_path_line_to(path, x, y + r);
  zui_path_quad_to(path, x, y, x + r, y);
  zui_path_close(path);

  return path;
}

ZuiPath *zui_path_circle(float cx, float cy, float radius)
{
  return zui_path_ellipse(cx, cy, radius, radius);
}

ZuiPath *zui_path_ellipse(float cx, float cy, float rx, float ry)
{
  ZuiPath *path = zui_path_new();
  if (!path) return NULL;

  float kappa = 0.5522847498f;
  float kx = rx * kappa;
  float ky = ry * kappa;

  zui_path_move_to(path, cx + rx, cy);
  zui_path_cubic_to(path, cx + rx, cy + ky, cx + kx, cy + ry, cx, cy + ry);
  zui_path_cubic_to(path, cx - kx, cy + ry, cx - rx, cy + ky, cx - rx, cy);
  zui_path_cubic_to(path, cx - rx, cy - ky, cx - kx, cy - ry, cx, cy - ry);
  zui_path_cubic_to(path, cx + kx, cy - ry, cx + rx, cy - ky, cx + rx, cy);
  zui_path_close(path);

  return path;
}

ZuiPath *zui_path_polygon(const float *points, int point_count)
{
  if (!points || point_count < 3) return NULL;

  ZuiPath *path = zui_path_new();
  if (!path) return NULL;

  zui_path_move_to(path, points[0], points[1]);
  for (int i = 1; i < point_count; i++) {
    zui_path_line_to(path, points[i * 2], points[i * 2 + 1]);
  }
  zui_path_close(path);

  return path;
}

ZuiPath *zui_path_regular_polygon(float cx, float cy, float radius, int sides)
{
  if (sides < 3) return NULL;

  ZuiPath *path = zui_path_new();
  if (!path) return NULL;

  float angle_step = (float)(2.0 * M_PI / sides);
  float start_angle = (float)(-M_PI / 2);

  for (int i = 0; i < sides; i++) {
    float angle = start_angle + (float)i * angle_step;
    float x = cx + radius * cosf(angle);
    float y = cy + radius * sinf(angle);

    if (i == 0) {
      zui_path_move_to(path, x, y);
    } else {
      zui_path_line_to(path, x, y);
    }
  }
  zui_path_close(path);

  return path;
}

ZuiPath *zui_path_star(float cx, float cy, float outer_radius, float inner_radius, int points)
{
  if (points < 3) return NULL;

  ZuiPath *path = zui_path_new();
  if (!path) return NULL;

  float angle_step = (float)(M_PI / points);
  float start_angle = (float)(-M_PI / 2);

  for (int i = 0; i < points * 2; i++) {
    float angle = start_angle + (float)i * angle_step;
    float r = (i % 2 == 0) ? outer_radius : inner_radius;
    float x = cx + r * cosf(angle);
    float y = cy + r * sinf(angle);

    if (i == 0) {
      zui_path_move_to(path, x, y);
    } else {
      zui_path_line_to(path, x, y);
    }
  }
  zui_path_close(path);

  return path;
}

ZuiPath *zui_path_rounded_polygon(const float *points, int point_count, float radius)
{
  if (!points || point_count < 3) return NULL;

  ZuiPath *path = zui_path_new();
  if (!path) return NULL;

  for (int i = 0; i < point_count; i++) {
    int prev = (i + point_count - 1) % point_count;
    int next = (i + 1) % point_count;

    float px = points[prev * 2];
    float py = points[prev * 2 + 1];
    float cx = points[i * 2];
    float cy = points[i * 2 + 1];
    float nx = points[next * 2];
    float ny = points[next * 2 + 1];

    float dx1 = px - cx;
    float dy1 = py - cy;
    float dx2 = nx - cx;
    float dy2 = ny - cy;

    float len1 = sqrtf(dx1 * dx1 + dy1 * dy1);
    float len2 = sqrtf(dx2 * dx2 + dy2 * dy2);

    if (len1 < 0.001f || len2 < 0.001f) continue;

    dx1 /= len1;
    dy1 /= len1;
    dx2 /= len2;
    dy2 /= len2;

    float r = radius;
    if (r > len1 / 2) r = len1 / 2;
    if (r > len2 / 2) r = len2 / 2;

    float start_x = cx + dx1 * r;
    float start_y = cy + dy1 * r;
    float end_x = cx + dx2 * r;
    float end_y = cy + dy2 * r;

    if (i == 0) {
      zui_path_move_to(path, start_x, start_y);
    } else {
      zui_path_line_to(path, start_x, start_y);
    }

    zui_path_quad_to(path, cx, cy, end_x, end_y);
  }
  zui_path_close(path);

  return path;
}

void zui_path_translate(ZuiPath *path, float dx, float dy)
{
  if (!path) return;

  for (int i = 0; i < path->count; i++) {
    ZuiPathCommand *cmd = &path->commands[i];
    switch (cmd->type) {
      case ZUI_PATH_MOVE_TO:
      case ZUI_PATH_LINE_TO:
      case ZUI_PATH_ARC_TO:
        cmd->x += dx;
        cmd->y += dy;
        break;
      case ZUI_PATH_QUAD_TO:
        cmd->x += dx;
        cmd->y += dy;
        cmd->cx1 += dx;
        cmd->cy1 += dy;
        break;
      case ZUI_PATH_CUBIC_TO:
        cmd->x += dx;
        cmd->y += dy;
        cmd->cx1 += dx;
        cmd->cy1 += dy;
        cmd->cx2 += dx;
        cmd->cy2 += dy;
        break;
      case ZUI_PATH_CLOSE:
        break;
    }
  }
  path->bounds_dirty = true;
}

void zui_path_scale(ZuiPath *path, float sx, float sy)
{
  if (!path) return;

  for (int i = 0; i < path->count; i++) {
    ZuiPathCommand *cmd = &path->commands[i];
    switch (cmd->type) {
      case ZUI_PATH_MOVE_TO:
      case ZUI_PATH_LINE_TO:
        cmd->x *= sx;
        cmd->y *= sy;
        break;
      case ZUI_PATH_ARC_TO:
        cmd->x *= sx;
        cmd->y *= sy;
        cmd->radius *= (sx + sy) / 2;
        break;
      case ZUI_PATH_QUAD_TO:
        cmd->x *= sx;
        cmd->y *= sy;
        cmd->cx1 *= sx;
        cmd->cy1 *= sy;
        break;
      case ZUI_PATH_CUBIC_TO:
        cmd->x *= sx;
        cmd->y *= sy;
        cmd->cx1 *= sx;
        cmd->cy1 *= sy;
        cmd->cx2 *= sx;
        cmd->cy2 *= sy;
        break;
      case ZUI_PATH_CLOSE:
        break;
    }
  }
  path->bounds_dirty = true;
}

void zui_path_rotate(ZuiPath *path, float angle, float cx, float cy)
{
  if (!path) return;

  float cos_a = cosf(angle);
  float sin_a = sinf(angle);

  for (int i = 0; i < path->count; i++) {
    ZuiPathCommand *cmd = &path->commands[i];

    float x, y;

    switch (cmd->type) {
      case ZUI_PATH_MOVE_TO:
      case ZUI_PATH_LINE_TO:
        x = cmd->x - cx;
        y = cmd->y - cy;
        cmd->x = x * cos_a - y * sin_a + cx;
        cmd->y = x * sin_a + y * cos_a + cy;
        break;
      case ZUI_PATH_ARC_TO:
        x = cmd->x - cx;
        y = cmd->y - cy;
        cmd->x = x * cos_a - y * sin_a + cx;
        cmd->y = x * sin_a + y * cos_a + cy;
        cmd->cx1 += angle;
        cmd->cy1 += angle;
        break;
      case ZUI_PATH_QUAD_TO:
        x = cmd->x - cx;
        y = cmd->y - cy;
        cmd->x = x * cos_a - y * sin_a + cx;
        cmd->y = x * sin_a + y * cos_a + cy;
        x = cmd->cx1 - cx;
        y = cmd->cy1 - cy;
        cmd->cx1 = x * cos_a - y * sin_a + cx;
        cmd->cy1 = x * sin_a + y * cos_a + cy;
        break;
      case ZUI_PATH_CUBIC_TO:
        x = cmd->x - cx;
        y = cmd->y - cy;
        cmd->x = x * cos_a - y * sin_a + cx;
        cmd->y = x * sin_a + y * cos_a + cy;
        x = cmd->cx1 - cx;
        y = cmd->cy1 - cy;
        cmd->cx1 = x * cos_a - y * sin_a + cx;
        cmd->cy1 = x * sin_a + y * cos_a + cy;
        x = cmd->cx2 - cx;
        y = cmd->cy2 - cy;
        cmd->cx2 = x * cos_a - y * sin_a + cx;
        cmd->cy2 = x * sin_a + y * cos_a + cy;
        break;
      case ZUI_PATH_CLOSE:
        break;
    }
  }
  path->bounds_dirty = true;
}

static void update_bounds(ZuiPath *path)
{
  if (!path || path->count == 0) {
    path->bounds_x = path->bounds_y = 0;
    path->bounds_w = path->bounds_h = 0;
    path->bounds_dirty = false;
    return;
  }

  float min_x = 1e9f, min_y = 1e9f;
  float max_x = -1e9f, max_y = -1e9f;

  for (int i = 0; i < path->count; i++) {
    ZuiPathCommand *cmd = &path->commands[i];

    switch (cmd->type) {
      case ZUI_PATH_MOVE_TO:
      case ZUI_PATH_LINE_TO:
        if (cmd->x < min_x) min_x = cmd->x;
        if (cmd->y < min_y) min_y = cmd->y;
        if (cmd->x > max_x) max_x = cmd->x;
        if (cmd->y > max_y) max_y = cmd->y;
        break;
      case ZUI_PATH_QUAD_TO:
        if (cmd->x < min_x) min_x = cmd->x;
        if (cmd->y < min_y) min_y = cmd->y;
        if (cmd->x > max_x) max_x = cmd->x;
        if (cmd->y > max_y) max_y = cmd->y;
        if (cmd->cx1 < min_x) min_x = cmd->cx1;
        if (cmd->cy1 < min_y) min_y = cmd->cy1;
        if (cmd->cx1 > max_x) max_x = cmd->cx1;
        if (cmd->cy1 > max_y) max_y = cmd->cy1;
        break;
      case ZUI_PATH_CUBIC_TO:
        if (cmd->x < min_x) min_x = cmd->x;
        if (cmd->y < min_y) min_y = cmd->y;
        if (cmd->x > max_x) max_x = cmd->x;
        if (cmd->y > max_y) max_y = cmd->y;
        if (cmd->cx1 < min_x) min_x = cmd->cx1;
        if (cmd->cy1 < min_y) min_y = cmd->cy1;
        if (cmd->cx1 > max_x) max_x = cmd->cx1;
        if (cmd->cy1 > max_y) max_y = cmd->cy1;
        if (cmd->cx2 < min_x) min_x = cmd->cx2;
        if (cmd->cy2 < min_y) min_y = cmd->cy2;
        if (cmd->cx2 > max_x) max_x = cmd->cx2;
        if (cmd->cy2 > max_y) max_y = cmd->cy2;
        break;
      case ZUI_PATH_ARC_TO:
        if (cmd->x - cmd->radius < min_x) min_x = cmd->x - cmd->radius;
        if (cmd->y - cmd->radius < min_y) min_y = cmd->y - cmd->radius;
        if (cmd->x + cmd->radius > max_x) max_x = cmd->x + cmd->radius;
        if (cmd->y + cmd->radius > max_y) max_y = cmd->y + cmd->radius;
        break;
      case ZUI_PATH_CLOSE:
        break;
    }
  }

  path->bounds_x = min_x;
  path->bounds_y = min_y;
  path->bounds_w = max_x - min_x;
  path->bounds_h = max_y - min_y;
  path->bounds_dirty = false;
}

void zui_path_get_bounds(ZuiPath *path, float *x, float *y, float *w, float *h)
{
  if (!path) {
    if (x) *x = 0;
    if (y) *y = 0;
    if (w) *w = 0;
    if (h) *h = 0;
    return;
  }

  if (path->bounds_dirty) {
    update_bounds(path);
  }

  if (x) *x = path->bounds_x;
  if (y) *y = path->bounds_y;
  if (w) *w = path->bounds_w;
  if (h) *h = path->bounds_h;
}

bool zui_path_contains(ZuiPath *path, float px, float py)
{
  if (!path || path->count == 0) return false;

  int crossings = 0;
  float start_x = 0, start_y = 0;
  float curr_x = 0, curr_y = 0;

  for (int i = 0; i < path->count; i++) {
    ZuiPathCommand *cmd = &path->commands[i];

    switch (cmd->type) {
      case ZUI_PATH_MOVE_TO:
        start_x = curr_x = cmd->x;
        start_y = curr_y = cmd->y;
        break;

      case ZUI_PATH_LINE_TO: {
        float x1 = curr_x, y1 = curr_y;
        float x2 = cmd->x, y2 = cmd->y;

        if ((y1 <= py && y2 > py) || (y2 <= py && y1 > py)) {
          float t = (py - y1) / (y2 - y1);
          float ix = x1 + t * (x2 - x1);
          if (px < ix) crossings++;
        }

        curr_x = cmd->x;
        curr_y = cmd->y;
        break;
      }

      case ZUI_PATH_CLOSE: {
        float x1 = curr_x, y1 = curr_y;
        float x2 = start_x, y2 = start_y;

        if ((y1 <= py && y2 > py) || (y2 <= py && y1 > py)) {
          float t = (py - y1) / (y2 - y1);
          float ix = x1 + t * (x2 - x1);
          if (px < ix) crossings++;
        }

        curr_x = start_x;
        curr_y = start_y;
        break;
      }

      default:
        curr_x = cmd->x;
        curr_y = cmd->y;
        break;
    }
  }

  return (crossings % 2) == 1;
}

const ZuiPathCommand *zui_path_get_commands(ZuiPath *path, int *count)
{
  if (!path) {
    if (count) *count = 0;
    return NULL;
  }
  if (count) *count = path->count;
  return path->commands;
}

void zui_draw_path(ZuiPath *path, ZuiColor fill_color)
{
  ZuiRenderer *renderer = zui_get_renderer();
  if (renderer && path) {
    zui_renderer_draw_path(renderer, path, fill_color);
  }
}

void zui_draw_path_stroke(ZuiPath *path, ZuiColor stroke_color, float thickness)
{
  ZuiRenderer *renderer = zui_get_renderer();
  if (renderer && path) {
    zui_renderer_draw_path_stroke(renderer, path, stroke_color, thickness);
  }
}
