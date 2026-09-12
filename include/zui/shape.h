#ifndef ZUI_SHAPE_H
#define ZUI_SHAPE_H

#include <zui/widget.h>
#include <zui/path.h>
#include <zui/color.h>

typedef struct ZuiShape ZuiShape;

#define ZUI_SHAPE(w) ((ZuiShape *)(w))

ZuiWidget *zui_shape_new(ZuiPath *path);
void zui_shape_destroy(ZuiWidget *widget);

void zui_shape_set_path(ZuiShape *shape, ZuiPath *path);
ZuiPath *zui_shape_get_path(ZuiShape *shape);

void zui_shape_set_fill(ZuiShape *shape, ZuiColor color);
ZuiColor zui_shape_get_fill(ZuiShape *shape);

void zui_shape_set_stroke(ZuiShape *shape, ZuiColor color, float thickness);
ZuiColor zui_shape_get_stroke_color(ZuiShape *shape);
float zui_shape_get_stroke_thickness(ZuiShape *shape);

void zui_shape_add_child(ZuiShape *shape, ZuiWidget *child);
void zui_shape_remove_child(ZuiShape *shape, ZuiWidget *child);

typedef void (*ZuiShapeClickCallback)(ZuiShape *shape, void *user_data);
typedef void (*ZuiShapeHoverCallback)(ZuiShape *shape, bool entered, void *user_data);

void zui_shape_on_click(ZuiShape *shape, ZuiShapeClickCallback callback, void *user_data);
void zui_shape_on_hover(ZuiShape *shape, ZuiShapeHoverCallback callback, void *user_data);

void zui_shape_set_cursor(ZuiShape *shape, ZuiCursor cursor);

#endif
