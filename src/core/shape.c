#include <zui/shape.h>
#include <zui/internal/widget_internal.h>
#include <zui/internal/renderer.h>
#include <stdlib.h>
#include <string.h>

extern ZuiRenderer *zui_get_renderer(void);

struct ZuiShape {
  ZuiWidget base;
  ZuiPath *path;
  ZuiPath *transformed_path;
  ZuiColor fill_color;
  ZuiColor stroke_color;
  float stroke_thickness;

  ZuiShapeClickCallback click_callback;
  void *click_user_data;
  ZuiShapeHoverCallback hover_callback;
  void *hover_user_data;

  ZuiCursor cursor;
  bool hovered;
  bool pressed;
};

static void shape_layout(ZuiWidget *widget);
static void shape_draw(ZuiWidget *widget, ZuiRenderer *renderer);
static bool shape_hit_test(ZuiWidget *widget, float x, float y);
static void shape_on_mouse_enter(ZuiWidget *widget);
static void shape_on_mouse_leave(ZuiWidget *widget);
static void shape_on_mouse_down(ZuiWidget *widget, float x, float y, uint32_t button);
static void shape_on_mouse_up(ZuiWidget *widget, float x, float y, uint32_t button);

static void shape_destroy(ZuiWidget *widget);

static ZuiWidgetVTable shape_vtable = {
  .layout = shape_layout,
  .draw = shape_draw,
  .hit_test = shape_hit_test,
  .on_mouse_enter = shape_on_mouse_enter,
  .on_mouse_leave = shape_on_mouse_leave,
  .on_mouse_down = shape_on_mouse_down,
  .on_mouse_up = shape_on_mouse_up,
  .destroy = shape_destroy,
};

static void update_transformed_path(ZuiShape *shape)
{
  if (!shape->path) return;

  if (shape->transformed_path) {
    zui_path_destroy(shape->transformed_path);
  }

  int count;
  const ZuiPathCommand *cmds = zui_path_get_commands(shape->path, &count);
  shape->transformed_path = zui_path_create(cmds, count);

  if (shape->transformed_path) {
    zui_path_translate(shape->transformed_path,
                       shape->base.bounds.x,
                       shape->base.bounds.y);
  }
}

ZuiWidget *zui_shape_new(ZuiPath *path)
{
  ZuiShape *shape = calloc(1, sizeof(ZuiShape));
  if (!shape) return NULL;

  shape->base.vtable = &shape_vtable;
  shape->base.visible = true;
  shape->path = path;
  shape->fill_color = ZUI_COLOR(0.5f, 0.5f, 0.5f, 1.0f);
  shape->stroke_color = ZUI_COLOR(0, 0, 0, 0);
  shape->stroke_thickness = 0;
  shape->cursor = ZUI_CURSOR_DEFAULT;

  if (path) {
    float x, y, w, h;
    zui_path_get_bounds(path, &x, &y, &w, &h);
    shape->base.bounds.width = w;
    shape->base.bounds.height = h;
  }

  return (ZuiWidget *)shape;
}

static void shape_destroy(ZuiWidget *widget)
{
  if (!widget) return;
  ZuiShape *shape = ZUI_SHAPE(widget);

  if (shape->transformed_path) {
    zui_path_destroy(shape->transformed_path);
  }
}

void zui_shape_destroy(ZuiWidget *widget)
{
  shape_destroy(widget);
}

void zui_shape_set_path(ZuiShape *shape, ZuiPath *path)
{
  if (!shape) return;
  shape->path = path;

  if (path) {
    float x, y, w, h;
    zui_path_get_bounds(path, &x, &y, &w, &h);
    shape->base.bounds.width = w;
    shape->base.bounds.height = h;
  }

  if (shape->transformed_path) {
    zui_path_destroy(shape->transformed_path);
    shape->transformed_path = NULL;
  }
}

ZuiPath *zui_shape_get_path(ZuiShape *shape)
{
  return shape ? shape->path : NULL;
}

void zui_shape_set_fill(ZuiShape *shape, ZuiColor color)
{
  if (shape) shape->fill_color = color;
}

ZuiColor zui_shape_get_fill(ZuiShape *shape)
{
  return shape ? shape->fill_color : ZUI_COLOR(0, 0, 0, 0);
}

void zui_shape_set_stroke(ZuiShape *shape, ZuiColor color, float thickness)
{
  if (!shape) return;
  shape->stroke_color = color;
  shape->stroke_thickness = thickness;
}

ZuiColor zui_shape_get_stroke_color(ZuiShape *shape)
{
  return shape ? shape->stroke_color : ZUI_COLOR(0, 0, 0, 0);
}

float zui_shape_get_stroke_thickness(ZuiShape *shape)
{
  return shape ? shape->stroke_thickness : 0;
}

void zui_shape_add_child(ZuiShape *shape, ZuiWidget *child)
{
  if (!shape || !child) return;
  zui_widget_add_child((ZuiWidget *)shape, child);
}

void zui_shape_remove_child(ZuiShape *shape, ZuiWidget *child)
{
  if (!shape || !child) return;

  for (int i = 0; i < shape->base.child_count; i++) {
    if (shape->base.children[i] == child) {
      child->parent = NULL;
      for (int j = i; j < shape->base.child_count - 1; j++) {
        shape->base.children[j] = shape->base.children[j + 1];
      }
      shape->base.child_count--;
      return;
    }
  }
}

void zui_shape_on_click(ZuiShape *shape, ZuiShapeClickCallback callback, void *user_data)
{
  if (!shape) return;
  shape->click_callback = callback;
  shape->click_user_data = user_data;
}

void zui_shape_on_hover(ZuiShape *shape, ZuiShapeHoverCallback callback, void *user_data)
{
  if (!shape) return;
  shape->hover_callback = callback;
  shape->hover_user_data = user_data;
}

void zui_shape_set_cursor(ZuiShape *shape, ZuiCursor cursor)
{
  if (shape) shape->cursor = cursor;
}

static void shape_layout(ZuiWidget *widget)
{
  ZuiShape *shape = ZUI_SHAPE(widget);

  update_transformed_path(shape);

  float cx = widget->bounds.x + widget->bounds.width / 2;
  float cy = widget->bounds.y + widget->bounds.height / 2;

  for (int i = 0; i < shape->base.child_count; i++) {
    ZuiWidget *child = shape->base.children[i];
    if (!child->visible) continue;

    float cw = child->bounds.width > 0 ? child->bounds.width : widget->bounds.width * 0.8f;
    float ch = child->bounds.height > 0 ? child->bounds.height : 20;

    child->bounds.x = cx - cw / 2;
    child->bounds.y = cy - ch / 2;
    child->bounds.width = cw;
    child->bounds.height = ch;

    if (child->vtable && child->vtable->layout) {
      child->vtable->layout(child);
    }
  }
}

static void shape_draw(ZuiWidget *widget, ZuiRenderer *renderer)
{
  ZuiShape *shape = ZUI_SHAPE(widget);

  if (!shape->transformed_path) {
    update_transformed_path(shape);
  }

  if (shape->transformed_path) {
    if (shape->fill_color.a > 0) {
      zui_renderer_draw_path(renderer, shape->transformed_path, shape->fill_color);
    }

    if (shape->stroke_thickness > 0 && shape->stroke_color.a > 0) {
      zui_renderer_draw_path_stroke(renderer, shape->transformed_path,
                                     shape->stroke_color, shape->stroke_thickness);
    }
  }

  if (shape->base.child_count > 0 && shape->transformed_path) {
    zui_renderer_push_path_clip(renderer, shape->transformed_path);

    for (int i = 0; i < shape->base.child_count; i++) {
      ZuiWidget *child = shape->base.children[i];
      if (child->visible && child->vtable && child->vtable->draw) {
        child->vtable->draw(child, renderer);
      }
    }

    zui_renderer_pop_path_clip(renderer);
  }
}

static bool shape_hit_test(ZuiWidget *widget, float x, float y)
{
  ZuiShape *shape = ZUI_SHAPE(widget);

  if (!shape->transformed_path) return false;

  return zui_path_contains(shape->transformed_path, x, y);
}

static void shape_on_mouse_enter(ZuiWidget *widget)
{
  ZuiShape *shape = ZUI_SHAPE(widget);
  shape->hovered = true;

  if (shape->hover_callback) {
    shape->hover_callback(shape, true, shape->hover_user_data);
  }

  if (shape->cursor != ZUI_CURSOR_DEFAULT) {
    zui_set_cursor(widget, shape->cursor);
  }
}

static void shape_on_mouse_leave(ZuiWidget *widget)
{
  ZuiShape *shape = ZUI_SHAPE(widget);
  shape->hovered = false;
  shape->pressed = false;

  if (shape->hover_callback) {
    shape->hover_callback(shape, false, shape->hover_user_data);
  }
}

static void shape_on_mouse_down(ZuiWidget *widget, float x, float y, uint32_t button)
{
  (void)x; (void)y; (void)button;
  ZuiShape *shape = ZUI_SHAPE(widget);
  shape->pressed = true;
}

static void shape_on_mouse_up(ZuiWidget *widget, float x, float y, uint32_t button)
{
  (void)x; (void)y; (void)button;
  ZuiShape *shape = ZUI_SHAPE(widget);

  if (shape->pressed && shape->hovered) {
    if (shape->click_callback) {
      shape->click_callback(shape, shape->click_user_data);
    }
  }

  shape->pressed = false;
}
