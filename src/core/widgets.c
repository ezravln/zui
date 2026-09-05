#include "zui/font.h"
#include <zui/internal/widget_internal.h>
#include <zui/internal/window_internal.h>
#include <zui/internal/icon_internal.h>
#include <zui/internal/font_internal.h>
#include <zui/internal/wayland_platform.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input-event-codes.h>
#include <xkbcommon/xkbcommon-keysyms.h>

extern ZuiPlatform *zui_get_platform(void);

extern void zui_widget_focus(ZuiWidget *widget);

extern ZuiRenderer *zui_get_renderer(void);

static ZuiFont *get_default_font(void);

extern void zui_window_close(ZuiWindow *window);
extern void zui_window_minimize(ZuiWindow *window);
extern void zui_window_maximize(ZuiWindow *window);

struct ZuiButton {
  ZuiWidget base;
  char *text;
  ZuiColor normal_color;
  ZuiColor hover_color;
  ZuiColor pressed_color;
  ZuiColor text_color;
  ZuiFont *font;
  ZuiTexture icon_texture;
  float icon_size;
  float icon_spacing;
};

typedef struct ZuiIconButton {
  ZuiButton base;
  ZuiIconSource *icon_source;
  ZuiTexture icon_texture;
  ZuiColor icon_color;
  float icon_size;
} ZuiIconButton;

typedef struct ZuiMaximizeButton {
  ZuiButton base;
  ZuiIconSource *maximize_icon;
  ZuiIconSource *restore_icon;
  ZuiTexture maximize_texture;
  ZuiTexture restore_texture;
  ZuiColor icon_color;
  float icon_size;
  ZuiWindow *window;
} ZuiMaximizeButton;

struct ZuiLabel {
  ZuiWidget base;
  char *text;
  ZuiColor text_color;
  ZuiFont *font;
  bool owns_font;
};

static void button_draw(ZuiWidget *widget, ZuiRenderer *renderer)
{
  ZuiButton *button = (ZuiButton *)widget;

  ZuiColor color = button->normal_color;
  if (widget->pressed) {
    color = button->pressed_color;
  } else if (widget->hovered) {
    color = button->hover_color;
  }

  zui_renderer_draw_rounded_rect(renderer,
    ZUI_RECT(widget->bounds.x, widget->bounds.y,
              widget->bounds.width, widget->bounds.height),
    color, widget->corner_radius);

  float content_width = 0;
  float text_w = 0, text_h = 0;

  if (button->icon_texture.id) {
    content_width += button->icon_size;
  }
  if (button->text && button->font) {
    text_w = zui_font_text_width(button->font, button->text);
    text_h = zui_font_text_height(button->font, button->text);
    if (button->icon_texture.id) {
      content_width += button->icon_spacing;
    }
    content_width += text_w;
  }

  float content_x = widget->bounds.x + (widget->bounds.width - content_width) / 2;
  float content_y = widget->bounds.y + widget->bounds.height / 2;

  if (button->icon_texture.id) {
    float icon_y = content_y - button->icon_size / 2;
    zui_renderer_draw_texture(renderer, &button->icon_texture,
      ZUI_RECT(content_x, icon_y, button->icon_size, button->icon_size),
      ZUI_COLOR_RGB(1.0f, 1.0f, 1.0f));
    content_x += button->icon_size + button->icon_spacing;
  }

  if (button->text && button->font) {
    float text_y = content_y - text_h / 2;
    zui_font_render_text(button->font, renderer, content_x, text_y,
                          button->text, button->text_color);
  }
}

static bool button_hit_test(ZuiWidget *widget, float x, float y)
{
  (void)x;
  (void)y;
  return true;
}

static void button_on_mouse_enter(ZuiWidget *widget)
{
  (void)widget;
}

static void button_on_mouse_leave(ZuiWidget *widget)
{
  (void)widget;
}

static void button_destroy(ZuiWidget *widget)
{
  ZuiButton *button = (ZuiButton *)widget;
  free(button->text);
  if (button->icon_texture.id) {
    zui_texture_destroy(&button->icon_texture);
  }
}

static const ZuiWidgetVTable button_vtable = {
  .draw = button_draw,
  .hit_test = button_hit_test,
  .on_mouse_enter = button_on_mouse_enter,
  .on_mouse_leave = button_on_mouse_leave,
  .destroy = button_destroy,
};

static void icon_button_draw(ZuiWidget *widget, ZuiRenderer *renderer)
{
  ZuiIconButton *btn = (ZuiIconButton *)widget;

  ZuiColor color = btn->base.normal_color;
  if (widget->pressed) {
    color = btn->base.pressed_color;
  } else if (widget->hovered) {
    color = btn->base.hover_color;
  }

  zui_renderer_draw_rounded_rect(renderer,
    ZUI_RECT(widget->bounds.x, widget->bounds.y,
              widget->bounds.width, widget->bounds.height),
    color, widget->corner_radius);

  if (btn->icon_source && btn->icon_texture.id) {
    float icon_x = widget->bounds.x + (widget->bounds.width - btn->icon_size) / 2;
    float icon_y = widget->bounds.y + (widget->bounds.height - btn->icon_size) / 2;
    zui_renderer_draw_texture(renderer, &btn->icon_texture,
                               ZUI_RECT(icon_x, icon_y, btn->icon_size, btn->icon_size),
                               btn->icon_color);
  }
}

static void icon_button_destroy(ZuiWidget *widget)
{
  ZuiIconButton *btn = (ZuiIconButton *)widget;
  if (btn->icon_texture.id) {
    zui_texture_destroy(&btn->icon_texture);
  }
  if (btn->icon_source) {
    zui_icon_source_destroy(btn->icon_source);
  }
  button_destroy(widget);
}

static const ZuiWidgetVTable icon_button_vtable = {
  .draw = icon_button_draw,
  .hit_test = button_hit_test,
  .on_mouse_enter = button_on_mouse_enter,
  .on_mouse_leave = button_on_mouse_leave,
  .destroy = icon_button_destroy,
};

extern bool zui_window_is_maximized(ZuiWindow *window);

static void maximize_button_draw(ZuiWidget *widget, ZuiRenderer *renderer)
{
  ZuiMaximizeButton *btn = (ZuiMaximizeButton *)widget;

  ZuiColor color = btn->base.normal_color;
  if (widget->pressed) {
    color = btn->base.pressed_color;
  } else if (widget->hovered) {
    color = btn->base.hover_color;
  }

  zui_renderer_draw_rounded_rect(renderer,
    ZUI_RECT(widget->bounds.x, widget->bounds.y,
              widget->bounds.width, widget->bounds.height),
    color, widget->corner_radius);

  ZuiTexture *tex = zui_window_is_maximized(btn->window)
                    ? &btn->restore_texture
                    : &btn->maximize_texture;

  if (tex && tex->id) {
    float icon_x = widget->bounds.x + (widget->bounds.width - btn->icon_size) / 2;
    float icon_y = widget->bounds.y + (widget->bounds.height - btn->icon_size) / 2;
    zui_renderer_draw_texture(renderer, tex,
                               ZUI_RECT(icon_x, icon_y, btn->icon_size, btn->icon_size),
                               btn->icon_color);
  }
}

static void maximize_button_destroy(ZuiWidget *widget)
{
  ZuiMaximizeButton *btn = (ZuiMaximizeButton *)widget;
  if (btn->maximize_texture.id) zui_texture_destroy(&btn->maximize_texture);
  if (btn->restore_texture.id) zui_texture_destroy(&btn->restore_texture);
  if (btn->maximize_icon) zui_icon_source_destroy(btn->maximize_icon);
  if (btn->restore_icon) zui_icon_source_destroy(btn->restore_icon);
  button_destroy(widget);
}

static const ZuiWidgetVTable maximize_button_vtable = {
  .draw = maximize_button_draw,
  .hit_test = button_hit_test,
  .on_mouse_enter = button_on_mouse_enter,
  .on_mouse_leave = button_on_mouse_leave,
  .destroy = maximize_button_destroy,
};

static ZuiIconButton *create_icon_button(const char *svg_path, float icon_size,
                                          ZuiColor icon_color)
{
  ZuiIconButton *btn = (ZuiIconButton *)zui_widget_create(
    sizeof(ZuiIconButton), ZUI_WIDGET_BUTTON, &icon_button_vtable);
  if (!btn) return NULL;

  btn->base.text = NULL;
  btn->base.normal_color = ZUI_COLOR_HEX(0x3d3d3d);
  btn->base.hover_color = ZUI_COLOR_HEX(0x4d4d4d);
  btn->base.pressed_color = ZUI_COLOR_HEX(0x2d2d2d);
  btn->base.base.corner_radius = 6.0f;
  btn->base.base.preferred_size.width = 20.0f;
  btn->base.base.preferred_size.height = 20.0f;
  btn->base.base.cursor = ZUI_CURSOR_POINTER;

  btn->icon_size = icon_size;
  btn->icon_color = icon_color;
  btn->icon_source = zui_icon_load_svg(svg_path);

  if (btn->icon_source) {
    btn->icon_texture = zui_texture_create(btn->icon_source->raster_data,
                                            btn->icon_source->raster_width,
                                            btn->icon_source->raster_height);
  }

  return btn;
}

ZuiButton *zui_button_create(const char *text)
{
  ZuiButton *button = (ZuiButton *)zui_widget_create(
    sizeof(ZuiButton), ZUI_WIDGET_BUTTON, &button_vtable);
  if (!button) return NULL;

  button->text = text ? strdup(text) : NULL;
  button->normal_color = ZUI_COLOR_HEX(0x3d3d3d);
  button->hover_color = ZUI_COLOR_HEX(0x4d4d4d);
  button->pressed_color = ZUI_COLOR_HEX(0x2d2d2d);
  button->text_color = ZUI_COLOR_HEX(0xffffff);
  button->font = get_default_font();
  button->icon_texture = (ZuiTexture){0};
  button->icon_size = 16.0f;
  button->icon_spacing = 8.0f;

  button->base.corner_radius = 6.0f;
  button->base.preferred_size.width = 80.0f;
  button->base.preferred_size.height = 32.0f;
  button->base.padding = 12.0f;
  button->base.cursor = ZUI_CURSOR_POINTER;

  return button;
}

void zui_button_set_text(ZuiButton *button, const char *text)
{
  if (!button) return;
  free(button->text);
  button->text = text ? strdup(text) : NULL;
}

void zui_button_set_size(ZuiButton *button, float width, float height)
{
  if (!button) return;
  button->base.preferred_size.width = width;
  button->base.preferred_size.height = height;
}

void zui_button_set_color(ZuiButton *button, ZuiColor color)
{
  if (!button) return;
  button->normal_color = color;
  button->hover_color = ZUI_COLOR(color.r * 1.2f, color.g * 1.2f,
                                   color.b * 1.2f, color.a);
  button->pressed_color = ZUI_COLOR(color.r * 0.8f, color.g * 0.8f,
                                     color.b * 0.8f, color.a);
}

void zui_button_on_click(ZuiButton *button, ZuiClickCallback callback,
                          void *user_data)
{
  if (!button) return;
  button->base.on_click = callback;
  button->base.user_data = user_data;
}

void zui_button_set_icon(ZuiButton *button, const char *image_path, float size)
{
  if (!button || !image_path) return;

  if (button->icon_texture.id) {
    zui_texture_destroy(&button->icon_texture);
  }

  button->icon_texture = zui_texture_load(image_path);
  button->icon_size = size;
}

void zui_button_set_text_color(ZuiButton *button, ZuiColor color)
{
  if (!button) return;
  button->text_color = color;
}

static void label_draw(ZuiWidget *widget, ZuiRenderer *renderer)
{
  ZuiLabel *label = (ZuiLabel *)widget;
  if (!label->text || !label->font) return;

  float text_w = zui_font_text_width(label->font, label->text);
  float text_h = zui_font_text_height(label->font, label->text);

  float x = widget->bounds.x + (widget->bounds.width - text_w) / 2;
  float y = widget->bounds.y + (widget->bounds.height - text_h) / 2;

  zui_font_render_text(label->font, renderer, x, y, label->text, label->text_color);
}

static void label_destroy(ZuiWidget *widget)
{
  ZuiLabel *label = (ZuiLabel *)widget;
  free(label->text);
  if (label->owns_font && label->font) {
    zui_font_destroy(label->font);
  }
}

static const ZuiWidgetVTable label_vtable = {
  .draw = label_draw,
  .destroy = label_destroy,
};

struct ZuiCheckbox {
  ZuiWidget base;
  char *label;
  bool checked;
  ZuiColor box_color;
  ZuiColor checked_box_color;
  ZuiColor icon_color;
  ZuiColor label_color;
  ZuiFont *font;
  float box_size;
  float corner_radius;
  ZuiIconSource *check_icon;
  ZuiTexture check_texture;
  ZuiCheckboxCallback on_change;
  void *change_user_data;
};

static void checkbox_draw(ZuiWidget *widget, ZuiRenderer *renderer)
{
  ZuiCheckbox *cb = (ZuiCheckbox *)widget;

  float box_x = widget->bounds.x;
  float box_y = widget->bounds.y + (widget->bounds.height - cb->box_size) / 2;

  ZuiColor box_color = cb->checked ? cb->checked_box_color : cb->box_color;
  if (widget->hovered) {
    box_color = ZUI_COLOR(box_color.r * 1.2f, box_color.g * 1.2f,
                           box_color.b * 1.2f, box_color.a);
  }

  zui_renderer_draw_rounded_rect(renderer,
    ZUI_RECT(box_x, box_y, cb->box_size, cb->box_size),
    box_color, cb->corner_radius);

  if (cb->checked && cb->check_texture.id) {
    float icon_size = cb->box_size * 0.85f;
    float icon_x = box_x + (cb->box_size - icon_size) / 2.0f;
    float icon_y = box_y + (cb->box_size - icon_size) / 2.0f;
    zui_renderer_draw_texture(renderer, &cb->check_texture,
      ZUI_RECT(icon_x, icon_y, icon_size, icon_size),
      cb->icon_color);
  }

  if (cb->label && cb->font) {
    float text_x = box_x + cb->box_size + 8.0f;
    float text_h = zui_font_text_height(cb->font, cb->label);
    float text_y = widget->bounds.y + (widget->bounds.height - text_h) / 2;
    zui_font_render_text(cb->font, renderer, text_x, text_y,
                          cb->label, cb->label_color);
  }
}

static bool checkbox_hit_test(ZuiWidget *widget, float x, float y)
{
  (void)x;
  (void)y;
  return true;
}

static void checkbox_on_mouse_enter(ZuiWidget *widget)
{
  (void)widget;
}

static void checkbox_on_mouse_leave(ZuiWidget *widget)
{
  (void)widget;
}

static void checkbox_on_mouse_up(ZuiWidget *widget, float x, float y,
                                  uint32_t button)
{
  (void)x;
  (void)y;
  if (button != BTN_LEFT) return;

  ZuiCheckbox *cb = (ZuiCheckbox *)widget;
  cb->checked = !cb->checked;

  if (cb->on_change) {
    cb->on_change(cb, cb->checked, cb->change_user_data);
  }
}

static void checkbox_destroy(ZuiWidget *widget)
{
  ZuiCheckbox *cb = (ZuiCheckbox *)widget;
  free(cb->label);
  if (cb->check_texture.id) {
    zui_texture_destroy(&cb->check_texture);
  }
  if (cb->check_icon) {
    zui_icon_source_destroy(cb->check_icon);
  }
}

static const ZuiWidgetVTable checkbox_vtable = {
  .draw = checkbox_draw,
  .hit_test = checkbox_hit_test,
  .on_mouse_enter = checkbox_on_mouse_enter,
  .on_mouse_leave = checkbox_on_mouse_leave,
  .on_mouse_up = checkbox_on_mouse_up,
  .destroy = checkbox_destroy,
};

static ZuiFont *get_default_font(void);

ZuiCheckbox *zui_checkbox_create(const char *label)
{
  ZuiCheckbox *cb = (ZuiCheckbox *)zui_widget_create(
    sizeof(ZuiCheckbox), ZUI_WIDGET_CHECKBOX, &checkbox_vtable);
  if (!cb) return NULL;

  cb->label = label ? strdup(label) : NULL;
  cb->checked = false;
  cb->box_color = ZUI_COLOR_HEX(0x4d4d4d);
  cb->checked_box_color = ZUI_COLOR_HEX(0x4a9eff);
  cb->icon_color = ZUI_COLOR_RGB(1.0f, 1.0f, 1.0f);
  cb->label_color = ZUI_COLOR_HEX(0xffffff);
  cb->font = get_default_font();
  cb->box_size = 18.0f;
  cb->corner_radius = 4.0f;
  cb->on_change = NULL;
  cb->change_user_data = NULL;

  cb->check_icon = zui_icon_load_svg("assets/icons/x-check-icon.svg");
  if (cb->check_icon) {
    cb->check_texture = zui_texture_create(cb->check_icon->raster_data,
                                            cb->check_icon->raster_width,
                                            cb->check_icon->raster_height);
  }

  cb->base.cursor = ZUI_CURSOR_POINTER;

  float label_width = 0;
  if (cb->font && label) {
    label_width = zui_font_text_width(cb->font, label);
  }
  cb->base.preferred_size.width = cb->box_size + 8.0f + label_width;
  cb->base.preferred_size.height = cb->box_size + 8.0f;

  return cb;
}

void zui_checkbox_set_checked(ZuiCheckbox *checkbox, bool checked)
{
  if (!checkbox) return;
  checkbox->checked = checked;
}

bool zui_checkbox_is_checked(ZuiCheckbox *checkbox)
{
  if (!checkbox) return false;
  return checkbox->checked;
}

void zui_checkbox_set_label(ZuiCheckbox *checkbox, const char *label)
{
  if (!checkbox) return;
  free(checkbox->label);
  checkbox->label = label ? strdup(label) : NULL;

  float label_width = 0;
  if (checkbox->font && label) {
    label_width = zui_font_text_width(checkbox->font, label);
  }
  checkbox->base.preferred_size.width = checkbox->box_size + 8.0f + label_width;
}

void zui_checkbox_set_size(ZuiCheckbox *checkbox, float size)
{
  if (!checkbox) return;
  checkbox->box_size = size;

  float label_width = 0;
  if (checkbox->font && checkbox->label) {
    label_width = zui_font_text_width(checkbox->font, checkbox->label);
  }
  checkbox->base.preferred_size.width = size + 8.0f + label_width;
  checkbox->base.preferred_size.height = size + 8.0f;
}

void zui_checkbox_on_change(ZuiCheckbox *checkbox, ZuiCheckboxCallback callback,
                             void *user_data)
{
  if (!checkbox) return;
  checkbox->on_change = callback;
  checkbox->change_user_data = user_data;
}

void zui_checkbox_set_box_color(ZuiCheckbox *checkbox, ZuiColor unchecked,
                                 ZuiColor checked)
{
  if (!checkbox) return;
  checkbox->box_color = unchecked;
  checkbox->checked_box_color = checked;
}

void zui_checkbox_set_icon_color(ZuiCheckbox *checkbox, ZuiColor color)
{
  if (!checkbox) return;
  checkbox->icon_color = color;
}

void zui_checkbox_set_icon(ZuiCheckbox *checkbox, const char *svg_path)
{
  if (!checkbox || !svg_path) return;

  if (checkbox->check_texture.id) {
    zui_texture_destroy(&checkbox->check_texture);
  }
  if (checkbox->check_icon) {
    zui_icon_source_destroy(checkbox->check_icon);
  }

  checkbox->check_icon = zui_icon_load_svg(svg_path);
  if (checkbox->check_icon) {
    checkbox->check_texture = zui_texture_create(
      checkbox->check_icon->raster_data,
      checkbox->check_icon->raster_width,
      checkbox->check_icon->raster_height);
  }
}

struct ZuiRadioGroup {
  ZuiRadioButton **buttons;
  size_t count;
  size_t capacity;
  ZuiRadioButton *selected;
  ZuiRadioCallback on_change;
  void *change_user_data;
};

struct ZuiRadioButton {
  ZuiWidget base;
  char *label;
  ZuiRadioGroup *group;
  ZuiColor circle_color;
  ZuiColor selected_color;
  ZuiColor dot_color;
  ZuiColor label_color;
  ZuiFont *font;
  float size;
};

static void radiobutton_draw(ZuiWidget *widget, ZuiRenderer *renderer)
{
  ZuiRadioButton *rb = (ZuiRadioButton *)widget;

  float cx = widget->bounds.x + rb->size / 2.0f;
  float cy = widget->bounds.y + widget->bounds.height / 2.0f;
  float radius = rb->size / 2.0f;

  bool selected = rb->group && rb->group->selected == rb;

  ZuiColor circle_color = selected ? rb->selected_color : rb->circle_color;
  if (widget->hovered) {
    circle_color = ZUI_COLOR(circle_color.r * 1.2f, circle_color.g * 1.2f,
                              circle_color.b * 1.2f, circle_color.a);
  }

  zui_renderer_draw_rounded_rect(renderer,
    ZUI_RECT(cx - radius, cy - radius, rb->size, rb->size),
    circle_color, radius);

  if (selected) {
    float dot_radius = radius * 0.5f;
    zui_renderer_draw_rounded_rect(renderer,
      ZUI_RECT(cx - dot_radius, cy - dot_radius, dot_radius * 2, dot_radius * 2),
      rb->dot_color, dot_radius);
  }

  if (rb->label && rb->font) {
    float text_x = widget->bounds.x + rb->size + 8.0f;
    float text_h = zui_font_text_height(rb->font, rb->label);
    float text_y = widget->bounds.y + (widget->bounds.height - text_h) / 2;
    zui_font_render_text(rb->font, renderer, text_x, text_y,
                          rb->label, rb->label_color);
  }
}

static bool radiobutton_hit_test(ZuiWidget *widget, float x, float y)
{
  (void)widget;
  (void)x;
  (void)y;
  return true;
}

static void radiobutton_on_mouse_up(ZuiWidget *widget, float x, float y,
                                     uint32_t button)
{
  (void)x;
  (void)y;
  if (button != BTN_LEFT) return;

  ZuiRadioButton *rb = (ZuiRadioButton *)widget;
  if (!rb->group) return;

  if (rb->group->selected != rb) {
    rb->group->selected = rb;
    if (rb->group->on_change) {
      rb->group->on_change(rb->group, rb, rb->group->change_user_data);
    }
  }
}

static void radiobutton_destroy(ZuiWidget *widget)
{
  ZuiRadioButton *rb = (ZuiRadioButton *)widget;
  free(rb->label);
}

static const ZuiWidgetVTable radiobutton_vtable = {
  .draw = radiobutton_draw,
  .hit_test = radiobutton_hit_test,
  .on_mouse_up = radiobutton_on_mouse_up,
  .destroy = radiobutton_destroy,
};

ZuiRadioGroup *zui_radiogroup_create(void)
{
  ZuiRadioGroup *group = calloc(1, sizeof(ZuiRadioGroup));
  if (!group) return NULL;

  group->buttons = NULL;
  group->count = 0;
  group->capacity = 0;
  group->selected = NULL;
  group->on_change = NULL;
  group->change_user_data = NULL;

  return group;
}

void zui_radiogroup_destroy(ZuiRadioGroup *group)
{
  if (!group) return;
  free(group->buttons);
  free(group);
}

void zui_radiogroup_on_change(ZuiRadioGroup *group, ZuiRadioCallback callback,
                               void *user_data)
{
  if (!group) return;
  group->on_change = callback;
  group->change_user_data = user_data;
}

ZuiRadioButton *zui_radiogroup_get_selected(ZuiRadioGroup *group)
{
  if (!group) return NULL;
  return group->selected;
}

int zui_radiogroup_get_selected_index(ZuiRadioGroup *group)
{
  if (!group || !group->selected) return -1;
  for (size_t i = 0; i < group->count; i++) {
    if (group->buttons[i] == group->selected) {
      return (int)i;
    }
  }
  return -1;
}

void zui_radiogroup_select(ZuiRadioGroup *group, size_t index)
{
  if (!group || index >= group->count) return;
  ZuiRadioButton *rb = group->buttons[index];
  if (group->selected != rb) {
    group->selected = rb;
    if (group->on_change) {
      group->on_change(group, rb, group->change_user_data);
    }
  }
}

ZuiRadioButton *zui_radiobutton_create(ZuiRadioGroup *group, const char *label)
{
  if (!group) return NULL;

  ZuiRadioButton *rb = (ZuiRadioButton *)zui_widget_create(
    sizeof(ZuiRadioButton), ZUI_WIDGET_RADIOBUTTON, &radiobutton_vtable);
  if (!rb) return NULL;

  rb->label = label ? strdup(label) : NULL;
  rb->group = group;
  rb->circle_color = ZUI_COLOR_HEX(0x4d4d4d);
  rb->selected_color = ZUI_COLOR_HEX(0x4a9eff);
  rb->dot_color = ZUI_COLOR_RGB(1.0f, 1.0f, 1.0f);
  rb->label_color = ZUI_COLOR_HEX(0xffffff);
  rb->font = get_default_font();
  rb->size = 18.0f;

  rb->base.cursor = ZUI_CURSOR_POINTER;

  float label_width = 0;
  if (rb->font && label) {
    label_width = zui_font_text_width(rb->font, label);
  }
  rb->base.preferred_size.width = rb->size + 8.0f + label_width;
  rb->base.preferred_size.height = rb->size + 8.0f;

  if (group->count >= group->capacity) {
    size_t new_cap = group->capacity == 0 ? 4 : group->capacity * 2;
    ZuiRadioButton **new_buttons = realloc(group->buttons,
                                            new_cap * sizeof(ZuiRadioButton *));
    if (!new_buttons) {
      zui_widget_destroy((ZuiWidget *)rb);
      return NULL;
    }
    group->buttons = new_buttons;
    group->capacity = new_cap;
  }

  group->buttons[group->count] = rb;
  group->count++;

  if (group->count == 1) {
    group->selected = rb;
  }

  return rb;
}

void zui_radiobutton_set_colors(ZuiRadioButton *rb, ZuiColor circle,
                                 ZuiColor selected, ZuiColor dot)
{
  if (!rb) return;
  rb->circle_color = circle;
  rb->selected_color = selected;
  rb->dot_color = dot;
}

void zui_radiobutton_set_label_color(ZuiRadioButton *rb, ZuiColor color)
{
  if (!rb) return;
  rb->label_color = color;
}

void zui_radiobutton_set_size(ZuiRadioButton *rb, float size)
{
  if (!rb) return;
  rb->size = size;

  float label_width = 0;
  if (rb->font && rb->label) {
    label_width = zui_font_text_width(rb->font, rb->label);
  }
  rb->base.preferred_size.width = size + 8.0f + label_width;
  rb->base.preferred_size.height = size + 8.0f;
}

const char *zui_radiobutton_get_label(ZuiRadioButton *rb)
{
  if (!rb) return NULL;
  return rb->label;
}

ZuiWidget *zui_radiobutton_as_widget(ZuiRadioButton *rb)
{
  return (ZuiWidget *)rb;
}

#define ZUI_TEXTINPUT_MAX_LEN 1024

struct ZuiTextInput {
  ZuiWidget base;
  char text[ZUI_TEXTINPUT_MAX_LEN];
  char *placeholder;
  size_t text_len;
  size_t cursor_pos;
  size_t selection_start;
  size_t selection_end;
  bool focused;
  bool dragging;
  ZuiFont *font;
  ZuiColor bg_color;
  ZuiColor text_color;
  ZuiColor placeholder_color;
  ZuiColor cursor_color;
  ZuiColor border_color;
  ZuiColor selection_color;
  float corner_radius;
  float padding;
  float scroll_offset;
  ZuiTextInputCallback on_change;
  void *change_user_data;
};

static void textinput_draw(ZuiWidget *widget, ZuiRenderer *renderer)
{
  ZuiTextInput *ti = (ZuiTextInput *)widget;

  ZuiColor border = ti->border_color;
  if (ti->focused) {
    border = ZUI_COLOR(border.r * 1.5f, border.g * 1.5f, border.b * 1.5f, border.a);
  }

  zui_renderer_draw_rounded_rect(renderer,
    ZUI_RECT(widget->bounds.x, widget->bounds.y,
             widget->bounds.width, widget->bounds.height),
    border, ti->corner_radius);

  zui_renderer_draw_rounded_rect(renderer,
    ZUI_RECT(widget->bounds.x + 1, widget->bounds.y + 1,
             widget->bounds.width - 2, widget->bounds.height - 2),
    ti->bg_color, ti->corner_radius - 1);

  float text_x = widget->bounds.x + ti->padding - ti->scroll_offset;
  float text_h = ti->font ? zui_font_text_height(ti->font, "Ay") : 14.0f;
  float text_y = widget->bounds.y + (widget->bounds.height - text_h) / 2;

  zui_renderer_push_clip(renderer,
    ZUI_RECT(widget->bounds.x + ti->padding, widget->bounds.y,
             widget->bounds.width - ti->padding * 2, widget->bounds.height),
    ti->corner_radius);

  if (ti->focused && ti->font && ti->selection_start != ti->selection_end) {
    size_t sel_min = ti->selection_start < ti->selection_end ?
                     ti->selection_start : ti->selection_end;
    size_t sel_max = ti->selection_start > ti->selection_end ?
                     ti->selection_start : ti->selection_end;

    char tmp[ZUI_TEXTINPUT_MAX_LEN];
    memcpy(tmp, ti->text, sel_min);
    tmp[sel_min] = '\0';
    float sel_start_x = text_x + zui_font_text_width(ti->font, tmp);

    memcpy(tmp, ti->text, sel_max);
    tmp[sel_max] = '\0';
    float sel_end_x = text_x + zui_font_text_width(ti->font, tmp);

    zui_renderer_draw_rect(renderer,
      ZUI_RECT(sel_start_x, text_y, sel_end_x - sel_start_x, text_h),
      ti->selection_color);
  }

  if (ti->text_len > 0 && ti->font) {
    zui_font_render_text(ti->font, renderer, text_x, text_y,
                          ti->text, ti->text_color);
  } else if (ti->placeholder && ti->font) {
    zui_font_render_text(ti->font, renderer, text_x, text_y,
                          ti->placeholder, ti->placeholder_color);
  }

  if (ti->focused && ti->font && ti->selection_start == ti->selection_end) {
    char tmp[ZUI_TEXTINPUT_MAX_LEN];
    size_t len = ti->cursor_pos < ti->text_len ? ti->cursor_pos : ti->text_len;
    memcpy(tmp, ti->text, len);
    tmp[len] = '\0';

    float cursor_x = text_x + zui_font_text_width(ti->font, tmp);
    float cursor_h = text_h;
    float cursor_y = text_y;

    zui_renderer_draw_rect(renderer,
      ZUI_RECT(cursor_x, cursor_y, 2.0f, cursor_h),
      ti->cursor_color);
  }

  zui_renderer_pop_clip(renderer);
}

static bool textinput_hit_test(ZuiWidget *widget, float x, float y)
{
  (void)widget;
  (void)x;
  (void)y;
  return true;
}

static size_t textinput_pos_from_x(ZuiTextInput *ti, float x);
static void textinput_update_scroll(ZuiTextInput *ti);

static void textinput_on_mouse_down(ZuiWidget *widget, float x, float y,
                                     uint32_t button)
{
  (void)y;
  if (button != BTN_LEFT) return;

  ZuiTextInput *ti = (ZuiTextInput *)widget;
  zui_widget_focus(widget);

  size_t pos = textinput_pos_from_x(ti, x);
  ti->cursor_pos = pos;
  ti->selection_start = ti->selection_end = pos;
  ti->dragging = true;
}

static void textinput_on_mouse_up(ZuiWidget *widget, float x, float y,
                                   uint32_t button)
{
  (void)x;
  (void)y;
  if (button != BTN_LEFT) return;

  ZuiTextInput *ti = (ZuiTextInput *)widget;
  ti->dragging = false;
}

static void textinput_on_mouse_move(ZuiWidget *widget, float x, float y)
{
  (void)y;
  ZuiTextInput *ti = (ZuiTextInput *)widget;
  if (!ti->dragging) return;

  size_t pos = textinput_pos_from_x(ti, x);
  ti->cursor_pos = pos;
  ti->selection_end = pos;
  textinput_update_scroll(ti);
}

static void textinput_on_focus(ZuiWidget *widget, bool focused)
{
  ZuiTextInput *ti = (ZuiTextInput *)widget;
  ti->focused = focused;
}

static size_t utf8_prev_char(const char *str, size_t pos)
{
  if (pos == 0) return 0;
  pos--;
  while (pos > 0 && (str[pos] & 0xC0) == 0x80) {
    pos--;
  }
  return pos;
}

static size_t utf8_next_char(const char *str, size_t pos, size_t len)
{
  if (pos >= len) return len;
  pos++;
  while (pos < len && (str[pos] & 0xC0) == 0x80) {
    pos++;
  }
  return pos;
}

static bool is_word_char(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9') || c == '_';
}

static size_t word_prev(const char *str, size_t pos)
{
  if (pos == 0) return 0;
  pos--;
  while (pos > 0 && !is_word_char(str[pos])) pos--;
  while (pos > 0 && is_word_char(str[pos - 1])) pos--;
  return pos;
}

static size_t word_next(const char *str, size_t pos, size_t len)
{
  if (pos >= len) return len;
  while (pos < len && !is_word_char(str[pos])) pos++;
  while (pos < len && is_word_char(str[pos])) pos++;
  return pos;
}

static void collect_textinputs(ZuiWidget *widget, ZuiWidget **list,
                                size_t *count, size_t max)
{
  if (!widget || !widget->visible) return;

  if (widget->type == ZUI_WIDGET_TEXTINPUT && *count < max) {
    list[(*count)++] = widget;
  }

  for (int i = 0; i < widget->child_count; i++) {
    collect_textinputs(widget->children[i], list, count, max);
  }
}

static ZuiWidget *find_parent_window_widget(ZuiWidget *widget)
{
  while (widget) {
    if (widget->type == ZUI_WIDGET_WINDOW) return widget;
    widget = widget->parent;
  }
  return NULL;
}

static ZuiWidget *find_adjacent_textinput(ZuiWidget *current, bool next)
{
  ZuiWidget *window = find_parent_window_widget(current);
  if (!window) return NULL;

  ZuiWidget *list[64];
  size_t count = 0;
  collect_textinputs(window, list, &count, 64);

  if (count < 2) return NULL;

  for (size_t i = 0; i < count; i++) {
    if (list[i] == current) {
      if (next && i + 1 < count) return list[i + 1];
      if (!next && i > 0) return list[i - 1];
      return NULL;
    }
  }
  return NULL;
}

static size_t textinput_pos_from_x(ZuiTextInput *ti, float x)
{
  if (!ti->font || ti->text_len == 0) return 0;

  float text_x = ti->base.bounds.x + ti->padding - ti->scroll_offset;
  float click_x = x - text_x;

  size_t pos = 0;
  char tmp[ZUI_TEXTINPUT_MAX_LEN];

  for (size_t i = 0; i <= ti->text_len; i++) {
    memcpy(tmp, ti->text, i);
    tmp[i] = '\0';
    float w = zui_font_text_width(ti->font, tmp);
    if (w >= click_x) {
      pos = i > 0 ? i - 1 : 0;
      float prev_w = 0;
      if (i > 0) {
        tmp[i - 1] = '\0';
        prev_w = zui_font_text_width(ti->font, tmp);
      }
      if (click_x - prev_w > w - click_x) {
        pos = i;
      }
      return pos;
    }
    pos = i;
  }
  return pos;
}

static void textinput_update_scroll(ZuiTextInput *ti)
{
  if (!ti->font) return;

  char tmp[ZUI_TEXTINPUT_MAX_LEN];
  size_t len = ti->cursor_pos < ti->text_len ? ti->cursor_pos : ti->text_len;
  memcpy(tmp, ti->text, len);
  tmp[len] = '\0';

  float cursor_x = zui_font_text_width(ti->font, tmp);
  float visible_width = ti->base.bounds.width - ti->padding * 2;

  if (cursor_x - ti->scroll_offset > visible_width) {
    ti->scroll_offset = cursor_x - visible_width + 10;
  } else if (cursor_x < ti->scroll_offset) {
    ti->scroll_offset = cursor_x > 10 ? cursor_x - 10 : 0;
  }
}

static bool textinput_has_selection(ZuiTextInput *ti)
{
  return ti->selection_start != ti->selection_end;
}

static void textinput_delete_selection(ZuiTextInput *ti)
{
  if (!textinput_has_selection(ti)) return;

  size_t sel_min = ti->selection_start < ti->selection_end ?
                   ti->selection_start : ti->selection_end;
  size_t sel_max = ti->selection_start > ti->selection_end ?
                   ti->selection_start : ti->selection_end;

  memmove(ti->text + sel_min, ti->text + sel_max,
          ti->text_len - sel_max + 1);
  ti->text_len -= (sel_max - sel_min);
  ti->cursor_pos = sel_min;
  ti->selection_start = ti->selection_end = ti->cursor_pos;
}

static void textinput_clear_selection(ZuiTextInput *ti)
{
  ti->selection_start = ti->selection_end = ti->cursor_pos;
}

static void textinput_on_key(ZuiWidget *widget, uint32_t key, uint32_t sym,
                              const char *text, bool pressed)
{
  (void)key;
  if (!pressed) return;

  ZuiTextInput *ti = (ZuiTextInput *)widget;
  ZuiPlatform *platform = zui_get_platform();
  bool ctrl = zui_platform_get_ctrl(platform);
  bool shift = zui_platform_get_shift(platform);

  if (sym == XKB_KEY_a && ctrl) {
    ti->selection_start = 0;
    ti->selection_end = ti->text_len;
    ti->cursor_pos = ti->text_len;
    textinput_update_scroll(ti);
    return;
  }

  if (sym == XKB_KEY_BackSpace) {
    if (textinput_has_selection(ti)) {
      textinput_delete_selection(ti);
      if (ti->on_change) {
        ti->on_change(ti, ti->text, ti->change_user_data);
      }
    } else if (ti->cursor_pos > 0) {
      size_t prev;
      if (ctrl) {
        prev = word_prev(ti->text, ti->cursor_pos);
      } else {
        prev = utf8_prev_char(ti->text, ti->cursor_pos);
      }
      size_t del_len = ti->cursor_pos - prev;
      memmove(ti->text + prev, ti->text + ti->cursor_pos,
              ti->text_len - ti->cursor_pos + 1);
      ti->text_len -= del_len;
      ti->cursor_pos = prev;
      textinput_clear_selection(ti);

      if (ti->on_change) {
        ti->on_change(ti, ti->text, ti->change_user_data);
      }
    }
  } else if (sym == XKB_KEY_Delete) {
    if (textinput_has_selection(ti)) {
      textinput_delete_selection(ti);
      if (ti->on_change) {
        ti->on_change(ti, ti->text, ti->change_user_data);
      }
    } else if (ti->cursor_pos < ti->text_len) {
      size_t next;
      if (ctrl) {
        next = word_next(ti->text, ti->cursor_pos, ti->text_len);
      } else {
        next = utf8_next_char(ti->text, ti->cursor_pos, ti->text_len);
      }
      size_t del_len = next - ti->cursor_pos;
      memmove(ti->text + ti->cursor_pos, ti->text + next,
              ti->text_len - next + 1);
      ti->text_len -= del_len;
      textinput_clear_selection(ti);

      if (ti->on_change) {
        ti->on_change(ti, ti->text, ti->change_user_data);
      }
    }
  } else if (sym == XKB_KEY_Left) {
    size_t new_pos;
    if (ctrl) {
      new_pos = word_prev(ti->text, ti->cursor_pos);
    } else {
      new_pos = utf8_prev_char(ti->text, ti->cursor_pos);
    }

    if (shift) {
      if (ti->selection_start == ti->selection_end) {
        ti->selection_start = ti->cursor_pos;
      }
      ti->selection_end = new_pos;
    } else {
      textinput_clear_selection(ti);
    }
    ti->cursor_pos = new_pos;
  } else if (sym == XKB_KEY_Right) {
    size_t new_pos;
    if (ctrl) {
      new_pos = word_next(ti->text, ti->cursor_pos, ti->text_len);
    } else {
      new_pos = utf8_next_char(ti->text, ti->cursor_pos, ti->text_len);
    }

    if (shift) {
      if (ti->selection_start == ti->selection_end) {
        ti->selection_start = ti->cursor_pos;
      }
      ti->selection_end = new_pos;
    } else {
      textinput_clear_selection(ti);
    }
    ti->cursor_pos = new_pos;
  } else if (sym == XKB_KEY_Up) {
    if (ctrl) {
      ZuiWidget *prev = find_adjacent_textinput(widget, false);
      if (prev) {
        zui_widget_focus(prev);
        return;
      }
    }
    if (shift) {
      if (ti->selection_start == ti->selection_end) {
        ti->selection_start = ti->cursor_pos;
      }
      ti->selection_end = 0;
    } else {
      textinput_clear_selection(ti);
    }
    ti->cursor_pos = 0;
  } else if (sym == XKB_KEY_Down) {
    if (ctrl) {
      ZuiWidget *next = find_adjacent_textinput(widget, true);
      if (next) {
        zui_widget_focus(next);
        return;
      }
    }
    if (shift) {
      if (ti->selection_start == ti->selection_end) {
        ti->selection_start = ti->cursor_pos;
      }
      ti->selection_end = ti->text_len;
    } else {
      textinput_clear_selection(ti);
    }
    ti->cursor_pos = ti->text_len;
  } else if (sym == XKB_KEY_Home) {
    if (shift) {
      if (ti->selection_start == ti->selection_end) {
        ti->selection_start = ti->cursor_pos;
      }
      ti->selection_end = 0;
    } else {
      textinput_clear_selection(ti);
    }
    ti->cursor_pos = 0;
  } else if (sym == XKB_KEY_End) {
    if (shift) {
      if (ti->selection_start == ti->selection_end) {
        ti->selection_start = ti->cursor_pos;
      }
      ti->selection_end = ti->text_len;
    } else {
      textinput_clear_selection(ti);
    }
    ti->cursor_pos = ti->text_len;
  } else if (text && text[0] && text[0] >= 0x20 && !ctrl) {
    if (textinput_has_selection(ti)) {
      textinput_delete_selection(ti);
    }

    size_t add_len = strlen(text);
    if (ti->text_len + add_len < ZUI_TEXTINPUT_MAX_LEN - 1) {
      memmove(ti->text + ti->cursor_pos + add_len,
              ti->text + ti->cursor_pos,
              ti->text_len - ti->cursor_pos + 1);
      memcpy(ti->text + ti->cursor_pos, text, add_len);
      ti->text_len += add_len;
      ti->cursor_pos += add_len;
      textinput_clear_selection(ti);

      if (ti->on_change) {
        ti->on_change(ti, ti->text, ti->change_user_data);
      }
    }
  }

  textinput_update_scroll(ti);
}

static void textinput_destroy(ZuiWidget *widget)
{
  ZuiTextInput *ti = (ZuiTextInput *)widget;
  free(ti->placeholder);
}

static const ZuiWidgetVTable textinput_vtable = {
  .draw = textinput_draw,
  .hit_test = textinput_hit_test,
  .on_mouse_down = textinput_on_mouse_down,
  .on_mouse_up = textinput_on_mouse_up,
  .on_mouse_move = textinput_on_mouse_move,
  .on_focus = textinput_on_focus,
  .on_key = textinput_on_key,
  .destroy = textinput_destroy,
};

ZuiTextInput *zui_textinput_create(const char *placeholder)
{
  ZuiTextInput *ti = (ZuiTextInput *)zui_widget_create(
    sizeof(ZuiTextInput), ZUI_WIDGET_TEXTINPUT, &textinput_vtable);
  if (!ti) return NULL;

  ti->text[0] = '\0';
  ti->placeholder = placeholder ? strdup(placeholder) : NULL;
  ti->text_len = 0;
  ti->cursor_pos = 0;
  ti->selection_start = 0;
  ti->selection_end = 0;
  ti->focused = false;
  ti->dragging = false;
  ti->font = get_default_font();
  ti->bg_color = ZUI_COLOR_HEX(0x1a1a1a);
  ti->text_color = ZUI_COLOR_HEX(0xffffff);
  ti->placeholder_color = ZUI_COLOR_HEX(0x666666);
  ti->cursor_color = ZUI_COLOR_HEX(0x4a9eff);
  ti->border_color = ZUI_COLOR_HEX(0x444444);
  ti->selection_color = ZUI_COLOR(0.29f, 0.62f, 1.0f, 0.4f);
  ti->corner_radius = 6.0f;
  ti->padding = 10.0f;
  ti->scroll_offset = 0.0f;
  ti->on_change = NULL;
  ti->change_user_data = NULL;

  ti->base.cursor = ZUI_CURSOR_TEXT;
  ti->base.preferred_size.width = 200.0f;
  ti->base.preferred_size.height = 36.0f;

  return ti;
}

void zui_textinput_set_text(ZuiTextInput *input, const char *text)
{
  if (!input) return;
  if (text) {
    size_t len = strlen(text);
    if (len >= ZUI_TEXTINPUT_MAX_LEN) len = ZUI_TEXTINPUT_MAX_LEN - 1;
    memcpy(input->text, text, len);
    input->text[len] = '\0';
    input->text_len = len;
    input->cursor_pos = len;
  } else {
    input->text[0] = '\0';
    input->text_len = 0;
    input->cursor_pos = 0;
  }
  input->scroll_offset = 0;
}

const char *zui_textinput_get_text(ZuiTextInput *input)
{
  if (!input) return "";
  return input->text;
}

void zui_textinput_set_placeholder(ZuiTextInput *input, const char *placeholder)
{
  if (!input) return;
  free(input->placeholder);
  input->placeholder = placeholder ? strdup(placeholder) : NULL;
}

void zui_textinput_set_size(ZuiTextInput *input, float width, float height)
{
  if (!input) return;
  input->base.preferred_size.width = width;
  input->base.preferred_size.height = height;
}

void zui_textinput_set_colors(ZuiTextInput *input, ZuiColor background,
                               ZuiColor text, ZuiColor placeholder,
                               ZuiColor cursor, ZuiColor border)
{
  if (!input) return;
  input->bg_color = background;
  input->text_color = text;
  input->placeholder_color = placeholder;
  input->cursor_color = cursor;
  input->border_color = border;
}

void zui_textinput_on_change(ZuiTextInput *input, ZuiTextInputCallback callback,
                              void *user_data)
{
  if (!input) return;
  input->on_change = callback;
  input->change_user_data = user_data;
}

ZuiWidget *zui_textinput_as_widget(ZuiTextInput *input)
{
  return (ZuiWidget *)input;
}

typedef void (*ZuiScrollViewCallback)(ZuiScrollView *sv, float x, float y,
                                       void *user_data);

struct ZuiScrollView {
  ZuiWidget base;
  ZuiWidget *content;
  float scroll_x;
  float scroll_y;
  float content_width;
  float content_height;
  ZuiScrollViewCallback on_scroll_cb;
  void *scroll_user_data;
};

static void scrollview_draw(ZuiWidget *widget, ZuiRenderer *renderer)
{
  ZuiScrollView *sv = (ZuiScrollView *)widget;

  if (widget->background.a > 0) {
    zui_renderer_draw_rounded_rect(renderer,
      ZUI_RECT(widget->bounds.x, widget->bounds.y,
               widget->bounds.width, widget->bounds.height),
      widget->background, widget->corner_radius);
  }

  if (!sv->content || !sv->content->visible) return;

  zui_renderer_push_clip(renderer,
    ZUI_RECT(widget->bounds.x, widget->bounds.y,
             widget->bounds.width, widget->bounds.height),
    widget->corner_radius);

  zui_widget_draw(sv->content, renderer);

  zui_renderer_pop_clip(renderer);
}

static void scrollview_layout(ZuiWidget *widget)
{
  ZuiScrollView *sv = (ZuiScrollView *)widget;
  if (!sv->content) return;

  float content_w = sv->content->preferred_size.width;
  float content_h = sv->content->preferred_size.height;

  if (sv->content->fill_width) {
    content_w = widget->bounds.width;
  }
  if (sv->content->fill_height) {
    content_h = widget->bounds.height;
  }

  sv->content_width = content_w;
  sv->content_height = content_h;

  float max_scroll_x = content_w - widget->bounds.width;
  float max_scroll_y = content_h - widget->bounds.height;
  if (max_scroll_x < 0) max_scroll_x = 0;
  if (max_scroll_y < 0) max_scroll_y = 0;

  if (sv->scroll_x > max_scroll_x) sv->scroll_x = max_scroll_x;
  if (sv->scroll_y > max_scroll_y) sv->scroll_y = max_scroll_y;
  if (sv->scroll_x < 0) sv->scroll_x = 0;
  if (sv->scroll_y < 0) sv->scroll_y = 0;

  zui_widget_set_bounds(sv->content,
    widget->bounds.x - sv->scroll_x,
    widget->bounds.y - sv->scroll_y,
    content_w, content_h);

  zui_widget_layout(sv->content);
}

static bool scrollview_hit_test(ZuiWidget *widget, float x, float y)
{
  (void)widget;
  (void)x;
  (void)y;
  return true;
}

static ZuiWidget *scrollview_hit_test_children(ZuiWidget *widget, float x, float y)
{
  ZuiScrollView *sv = (ZuiScrollView *)widget;
  if (sv->content && sv->content->visible) {
    return zui_widget_hit_test(sv->content, x, y);
  }
  return NULL;
}

static void scrollview_on_scroll(ZuiWidget *widget, double dx, double dy)
{
  ZuiScrollView *sv = (ZuiScrollView *)widget;

  sv->scroll_x += (float)dx;
  sv->scroll_y += (float)dy;

  float max_scroll_x = sv->content_width - widget->bounds.width;
  float max_scroll_y = sv->content_height - widget->bounds.height;
  if (max_scroll_x < 0) max_scroll_x = 0;
  if (max_scroll_y < 0) max_scroll_y = 0;

  if (sv->scroll_x > max_scroll_x) sv->scroll_x = max_scroll_x;
  if (sv->scroll_y > max_scroll_y) sv->scroll_y = max_scroll_y;
  if (sv->scroll_x < 0) sv->scroll_x = 0;
  if (sv->scroll_y < 0) sv->scroll_y = 0;

  if (sv->on_scroll_cb) {
    sv->on_scroll_cb(sv, sv->scroll_x, sv->scroll_y, sv->scroll_user_data);
  }

  widget->needs_layout = true;
}

static void scrollview_destroy(ZuiWidget *widget)
{
  ZuiScrollView *sv = (ZuiScrollView *)widget;
  if (sv->content) {
    zui_widget_destroy(sv->content);
  }
}

static const ZuiWidgetVTable scrollview_vtable = {
  .draw = scrollview_draw,
  .layout = scrollview_layout,
  .hit_test = scrollview_hit_test,
  .hit_test_children = scrollview_hit_test_children,
  .on_scroll = scrollview_on_scroll,
  .destroy = scrollview_destroy,
};

ZuiScrollView *zui_scrollview_create(void)
{
  ZuiScrollView *sv = (ZuiScrollView *)zui_widget_create(
    sizeof(ZuiScrollView), ZUI_WIDGET_SCROLLVIEW, &scrollview_vtable);
  if (!sv) return NULL;

  sv->content = NULL;
  sv->scroll_x = 0;
  sv->scroll_y = 0;
  sv->content_width = 0;
  sv->content_height = 0;
  sv->on_scroll_cb = NULL;
  sv->scroll_user_data = NULL;

  sv->base.preferred_size.width = 200.0f;
  sv->base.preferred_size.height = 200.0f;

  return sv;
}

void zui_scrollview_set_content(ZuiScrollView *sv, ZuiWidget *content)
{
  if (!sv) return;
  if (sv->content) {
    zui_widget_destroy(sv->content);
  }
  sv->content = content;
  if (content) {
    content->parent = (ZuiWidget *)sv;
  }
}

void zui_scrollview_set_size(ZuiScrollView *sv, float width, float height)
{
  if (!sv) return;
  sv->base.preferred_size.width = width;
  sv->base.preferred_size.height = height;
}

void zui_scrollview_scroll_to(ZuiScrollView *sv, float x, float y)
{
  if (!sv) return;
  sv->scroll_x = x;
  sv->scroll_y = y;
  sv->base.needs_layout = true;
}

void zui_scrollview_get_scroll(ZuiScrollView *sv, float *x, float *y)
{
  if (!sv) return;
  if (x) *x = sv->scroll_x;
  if (y) *y = sv->scroll_y;
}

void zui_scrollview_on_scroll(ZuiScrollView *sv, ZuiScrollViewCallback callback,
                               void *user_data)
{
  if (!sv) return;
  sv->on_scroll_cb = callback;
  sv->scroll_user_data = user_data;
}

ZuiWidget *zui_scrollview_as_widget(ZuiScrollView *sv)
{
  return (ZuiWidget *)sv;
}

struct ZuiScroller {
  ZuiWidget base;
  bool vertical;
  float content_size;
  float viewport_size;
  float value;
  float thumb_drag_offset;
  bool dragging;
  ZuiColor track_color;
  ZuiColor thumb_color;
  ZuiColor thumb_hover_color;
  ZuiColor thumb_drag_color;
  ZuiScrollerCallback on_change;
  void *change_user_data;
};

static float scroller_get_thumb_size(ZuiScroller *sc)
{
  if (sc->content_size <= sc->viewport_size) {
    return sc->vertical ? sc->base.bounds.height : sc->base.bounds.width;
  }

  float track_size = sc->vertical ? sc->base.bounds.height : sc->base.bounds.width;
  float ratio = sc->viewport_size / sc->content_size;
  float thumb_size = track_size * ratio;

  float min_thumb = 30.0f;
  if (thumb_size < min_thumb) thumb_size = min_thumb;
  if (thumb_size > track_size) thumb_size = track_size;

  return thumb_size;
}

static float scroller_get_max_value(ZuiScroller *sc)
{
  float max_val = sc->content_size - sc->viewport_size;
  return max_val > 0 ? max_val : 0;
}

static float scroller_get_thumb_pos(ZuiScroller *sc)
{
  float max_val = scroller_get_max_value(sc);
  if (max_val <= 0) return 0;

  float track_size = sc->vertical ? sc->base.bounds.height : sc->base.bounds.width;
  float thumb_size = scroller_get_thumb_size(sc);
  float available = track_size - thumb_size;

  return (sc->value / max_val) * available;
}

static void scroller_draw(ZuiWidget *widget, ZuiRenderer *renderer)
{
  ZuiScroller *sc = (ZuiScroller *)widget;

  zui_renderer_draw_rounded_rect(renderer,
    ZUI_RECT(widget->bounds.x, widget->bounds.y,
             widget->bounds.width, widget->bounds.height),
    sc->track_color, widget->corner_radius);

  float thumb_size = scroller_get_thumb_size(sc);
  float thumb_pos = scroller_get_thumb_pos(sc);

  ZuiColor thumb_color = sc->thumb_color;
  if (sc->dragging) {
    thumb_color = sc->thumb_drag_color;
  } else if (widget->hovered) {
    thumb_color = sc->thumb_hover_color;
  }

  float thumb_x, thumb_y, thumb_w, thumb_h;
  if (sc->vertical) {
    thumb_x = widget->bounds.x + 2.0f;
    thumb_y = widget->bounds.y + thumb_pos;
    thumb_w = widget->bounds.width - 4.0f;
    thumb_h = thumb_size;
  } else {
    thumb_x = widget->bounds.x + thumb_pos;
    thumb_y = widget->bounds.y + 2.0f;
    thumb_w = thumb_size;
    thumb_h = widget->bounds.height - 4.0f;
  }

  zui_renderer_draw_rounded_rect(renderer,
    ZUI_RECT(thumb_x, thumb_y, thumb_w, thumb_h),
    thumb_color, widget->corner_radius - 1.0f);
}

static bool scroller_hit_test(ZuiWidget *widget, float x, float y)
{
  (void)widget;
  (void)x;
  (void)y;
  return true;
}

static void scroller_on_mouse_enter(ZuiWidget *widget)
{
  (void)widget;
}

static void scroller_on_mouse_leave(ZuiWidget *widget)
{
  (void)widget;
}

static void scroller_on_mouse_down(ZuiWidget *widget, float x, float y,
                                    uint32_t button)
{
  if (button != BTN_LEFT) return;

  ZuiScroller *sc = (ZuiScroller *)widget;

  float thumb_size = scroller_get_thumb_size(sc);
  float thumb_pos = scroller_get_thumb_pos(sc);

  float local_pos = sc->vertical
    ? (y - widget->bounds.y)
    : (x - widget->bounds.x);

  if (local_pos >= thumb_pos && local_pos <= thumb_pos + thumb_size) {
    sc->dragging = true;
    sc->thumb_drag_offset = local_pos - thumb_pos;
  } else {
    float track_size = sc->vertical ? widget->bounds.height : widget->bounds.width;
    float available = track_size - thumb_size;
    float max_val = scroller_get_max_value(sc);

    if (available > 0 && max_val > 0) {
      float new_thumb_pos = local_pos - thumb_size / 2;
      if (new_thumb_pos < 0) new_thumb_pos = 0;
      if (new_thumb_pos > available) new_thumb_pos = available;

      sc->value = (new_thumb_pos / available) * max_val;

      if (sc->on_change) {
        sc->on_change(sc, sc->value, sc->change_user_data);
      }
    }

    sc->dragging = true;
    sc->thumb_drag_offset = thumb_size / 2;
  }
}

static void scroller_on_mouse_up(ZuiWidget *widget, float x, float y,
                                  uint32_t button)
{
  (void)x;
  (void)y;
  if (button != BTN_LEFT) return;

  ZuiScroller *sc = (ZuiScroller *)widget;
  sc->dragging = false;
}

static void scroller_on_mouse_move(ZuiWidget *widget, float x, float y)
{
  ZuiScroller *sc = (ZuiScroller *)widget;
  if (!sc->dragging) return;

  float local_pos = sc->vertical
    ? (y - widget->bounds.y)
    : (x - widget->bounds.x);

  float thumb_size = scroller_get_thumb_size(sc);
  float track_size = sc->vertical ? widget->bounds.height : widget->bounds.width;
  float available = track_size - thumb_size;
  float max_val = scroller_get_max_value(sc);

  if (available > 0 && max_val > 0) {
    float new_thumb_pos = local_pos - sc->thumb_drag_offset;
    if (new_thumb_pos < 0) new_thumb_pos = 0;
    if (new_thumb_pos > available) new_thumb_pos = available;

    float new_value = (new_thumb_pos / available) * max_val;
    if (new_value != sc->value) {
      sc->value = new_value;
      if (sc->on_change) {
        sc->on_change(sc, sc->value, sc->change_user_data);
      }
    }
  }
}

static void scroller_destroy(ZuiWidget *widget)
{
  (void)widget;
}

static const ZuiWidgetVTable scroller_vtable = {
  .draw = scroller_draw,
  .hit_test = scroller_hit_test,
  .on_mouse_enter = scroller_on_mouse_enter,
  .on_mouse_leave = scroller_on_mouse_leave,
  .on_mouse_down = scroller_on_mouse_down,
  .on_mouse_up = scroller_on_mouse_up,
  .on_mouse_move = scroller_on_mouse_move,
  .destroy = scroller_destroy,
};

ZuiScroller *zui_scroller_create(bool vertical)
{
  ZuiScroller *sc = (ZuiScroller *)zui_widget_create(
    sizeof(ZuiScroller), ZUI_WIDGET_SCROLLER, &scroller_vtable);
  if (!sc) return NULL;

  sc->vertical = vertical;
  sc->content_size = 100.0f;
  sc->viewport_size = 100.0f;
  sc->value = 0.0f;
  sc->thumb_drag_offset = 0.0f;
  sc->dragging = false;

  sc->track_color = ZUI_COLOR_HEX(0x2d2d2d);
  sc->thumb_color = ZUI_COLOR_HEX(0x5a5a5a);
  sc->thumb_hover_color = ZUI_COLOR_HEX(0x6a6a6a);
  sc->thumb_drag_color = ZUI_COLOR_HEX(0x7a7a7a);

  sc->on_change = NULL;
  sc->change_user_data = NULL;

  sc->base.corner_radius = 4.0f;
  sc->base.cursor = ZUI_CURSOR_POINTER;

  if (vertical) {
    sc->base.preferred_size.width = 10.0f;
    sc->base.preferred_size.height = 100.0f;
  } else {
    sc->base.preferred_size.width = 100.0f;
    sc->base.preferred_size.height = 10.0f;
  }

  return sc;
}

void zui_scroller_set_size(ZuiScroller *scroller, float width, float height)
{
  if (!scroller) return;
  scroller->base.preferred_size.width = width;
  scroller->base.preferred_size.height = height;
}

void zui_scroller_set_range(ZuiScroller *scroller, float content_size,
                             float viewport_size)
{
  if (!scroller) return;
  scroller->content_size = content_size;
  scroller->viewport_size = viewport_size;

  float max_val = scroller_get_max_value(scroller);
  if (scroller->value > max_val) {
    scroller->value = max_val;
  }
}

void zui_scroller_set_value(ZuiScroller *scroller, float value)
{
  if (!scroller) return;

  float max_val = scroller_get_max_value(scroller);
  if (value < 0) value = 0;
  if (value > max_val) value = max_val;

  scroller->value = value;
}

float zui_scroller_get_value(ZuiScroller *scroller)
{
  if (!scroller) return 0.0f;
  return scroller->value;
}

void zui_scroller_on_change(ZuiScroller *scroller, ZuiScrollerCallback callback,
                             void *user_data)
{
  if (!scroller) return;
  scroller->on_change = callback;
  scroller->change_user_data = user_data;
}

ZuiWidget *zui_scroller_as_widget(ZuiScroller *scroller)
{
  return (ZuiWidget *)scroller;
}

struct ZuiSplitView {
  ZuiWidget base;
  bool vertical;
  ZuiWidget **children;
  size_t child_count;
  size_t child_capacity;
  float *positions;
  float min_child_size;
  float handle_size;
  float grip_length;
  int dragging_handle;
  float drag_offset;
  int hovered_handle;
  ZuiSplitAnchor anchor;
  float last_total;
  ZuiColor handle_color;
  ZuiColor grip_color;
  ZuiColor grip_hover_color;
  ZuiColor grip_drag_color;
};

static void splitview_layout(ZuiWidget *widget)
{
  ZuiSplitView *sv = (ZuiSplitView *)widget;

  if (sv->child_count == 0) return;

  float total = sv->vertical ? widget->bounds.height : widget->bounds.width;
  float handle = sv->handle_size;
  size_t handle_count = sv->child_count > 1 ? sv->child_count - 1 : 0;

  if (sv->child_count == 1) {
    ZuiWidget *child = sv->children[0];
    if (child) {
      zui_widget_set_bounds(child, widget->bounds.x, widget->bounds.y,
                             widget->bounds.width, widget->bounds.height);
      zui_widget_layout(child);
    }
    sv->last_total = total;
    return;
  }

  float total_handle_space = (float)handle_count * handle;
  float available = total - total_handle_space;

  if (sv->last_total > 0.0f && total != sv->last_total) {
    float old_available = sv->last_total - total_handle_space;
    float delta = available - old_available;

    switch (sv->anchor) {
      case ZUI_SPLIT_ANCHOR_START:
        break;

      case ZUI_SPLIT_ANCHOR_END:
        for (size_t i = 0; i < handle_count; i++) {
          sv->positions[i] += delta;
        }
        break;

      case ZUI_SPLIT_ANCHOR_CENTER:
        for (size_t i = 0; i < handle_count; i++) {
          sv->positions[i] += delta / 2.0f;
        }
        break;

      case ZUI_SPLIT_ANCHOR_AUTO:
      default:
        if (old_available > 0.0f) {
          float scale = available / old_available;
          for (size_t i = 0; i < handle_count; i++) {
            sv->positions[i] *= scale;
          }
        }
        break;
    }
  }

  sv->last_total = total;

  for (size_t i = 0; i < handle_count; i++) {
    float min_pos = sv->min_child_size;
    if (i > 0) {
      min_pos = sv->positions[i - 1] + handle + sv->min_child_size;
    }

    float max_pos = available;
    for (size_t j = i + 1; j < handle_count; j++) {
      max_pos -= sv->min_child_size;
    }
    max_pos -= sv->min_child_size;

    if (sv->positions[i] < min_pos) sv->positions[i] = min_pos;
    if (sv->positions[i] > max_pos) sv->positions[i] = max_pos;
  }

  float offset = 0.0f;
  for (size_t i = 0; i < sv->child_count; i++) {
    ZuiWidget *child = sv->children[i];
    if (!child) {
      if (i < handle_count) {
        offset = sv->positions[i] + handle;
      }
      continue;
    }

    float child_size;
    if (i == 0) {
      child_size = sv->positions[0];
    } else if (i < handle_count) {
      child_size = sv->positions[i] - sv->positions[i - 1] - handle;
    } else {
      child_size = available - sv->positions[i - 1];
    }

    float x, y, w, h;
    if (sv->vertical) {
      x = widget->bounds.x;
      y = widget->bounds.y + offset;
      w = widget->bounds.width;
      h = child_size;
    } else {
      x = widget->bounds.x + offset;
      y = widget->bounds.y;
      w = child_size;
      h = widget->bounds.height;
    }

    zui_widget_set_bounds(child, x, y, w, h);
    zui_widget_layout(child);

    offset += child_size + handle;
  }
}

static void splitview_draw(ZuiWidget *widget, ZuiRenderer *renderer)
{
  ZuiSplitView *sv = (ZuiSplitView *)widget;

  for (size_t i = 0; i < sv->child_count; i++) {
    ZuiWidget *child = sv->children[i];
    if (child && child->visible) {
      zui_widget_draw(child, renderer);
    }
  }

  if (sv->child_count < 2) return;

  size_t handle_count = sv->child_count - 1;

  for (size_t i = 0; i < handle_count; i++) {
    float handle_pos = sv->positions[i];

    float hx, hy, hw, hh;
    if (sv->vertical) {
      hx = widget->bounds.x;
      hy = widget->bounds.y + handle_pos;
      hw = widget->bounds.width;
      hh = sv->handle_size;
    } else {
      hx = widget->bounds.x + handle_pos;
      hy = widget->bounds.y;
      hw = sv->handle_size;
      hh = widget->bounds.height;
    }

    if (sv->handle_color.a > 0.0f) {
      zui_renderer_draw_rounded_rect(renderer,
        ZUI_RECT(hx, hy, hw, hh), sv->handle_color, 0.0f);
    }

    if (sv->grip_color.a > 0.0f) {
      ZuiColor grip = sv->grip_color;
      if (sv->dragging_handle == (int)i) {
        grip = sv->grip_drag_color;
      } else if (sv->hovered_handle == (int)i) {
        grip = sv->grip_hover_color;
      }

      float gx, gy, gw, gh, gr;
      if (sv->vertical) {
        gw = sv->grip_length;
        gh = sv->handle_size - 2.0f;
        gx = hx + (hw - gw) / 2.0f;
        gy = hy + (hh - gh) / 2.0f;
        gr = gh / 2.0f;
      } else {
        gw = sv->handle_size - 2.0f;
        gh = sv->grip_length;
        gx = hx + (hw - gw) / 2.0f;
        gy = hy + (hh - gh) / 2.0f;
        gr = gw / 2.0f;
      }

      zui_renderer_draw_rounded_rect(renderer,
        ZUI_RECT(gx, gy, gw, gh), grip, gr);
    }
  }
}

static int splitview_handle_at(ZuiSplitView *sv, float x, float y)
{
  if (sv->child_count < 2) return -1;

  ZuiWidget *widget = (ZuiWidget *)sv;
  size_t handle_count = sv->child_count - 1;

  for (size_t i = 0; i < handle_count; i++) {
    float handle_pos = sv->positions[i];
    float hx, hy, hw, hh;

    if (sv->vertical) {
      hx = widget->bounds.x;
      hy = widget->bounds.y + handle_pos;
      hw = widget->bounds.width;
      hh = sv->handle_size;
    } else {
      hx = widget->bounds.x + handle_pos;
      hy = widget->bounds.y;
      hw = sv->handle_size;
      hh = widget->bounds.height;
    }

    if (x >= hx && x < hx + hw && y >= hy && y < hy + hh) {
      return (int)i;
    }
  }

  return -1;
}

static bool splitview_hit_test(ZuiWidget *widget, float x, float y)
{
  (void)widget;
  (void)x;
  (void)y;
  return true;
}

static ZuiWidget *splitview_hit_test_children(ZuiWidget *widget, float x, float y)
{
  ZuiSplitView *sv = (ZuiSplitView *)widget;

  int handle = splitview_handle_at(sv, x, y);
  if (handle >= 0) {
    sv->hovered_handle = handle;
    return NULL;
  }

  sv->hovered_handle = -1;

  for (size_t i = 0; i < sv->child_count; i++) {
    ZuiWidget *child = sv->children[i];
    if (child && child->visible) {
      ZuiWidget *hit = zui_widget_hit_test(child, x, y);
      if (hit) return hit;
    }
  }

  return NULL;
}

static void splitview_on_mouse_enter(ZuiWidget *widget)
{
  (void)widget;
}

static void splitview_on_mouse_leave(ZuiWidget *widget)
{
  ZuiSplitView *sv = (ZuiSplitView *)widget;
  sv->hovered_handle = -1;
}

static void splitview_on_mouse_down(ZuiWidget *widget, float x, float y,
                                     uint32_t button)
{
  if (button != BTN_LEFT) return;

  ZuiSplitView *sv = (ZuiSplitView *)widget;

  int handle = splitview_handle_at(sv, x, y);
  if (handle >= 0) {
    sv->dragging_handle = handle;
    if (sv->vertical) {
      sv->drag_offset = y - (widget->bounds.y + sv->positions[handle]);
    } else {
      sv->drag_offset = x - (widget->bounds.x + sv->positions[handle]);
    }
  }
}

static void splitview_on_mouse_up(ZuiWidget *widget, float x, float y,
                                   uint32_t button)
{
  (void)x;
  (void)y;
  if (button != BTN_LEFT) return;

  ZuiSplitView *sv = (ZuiSplitView *)widget;
  sv->dragging_handle = -1;
}

static void splitview_on_mouse_move(ZuiWidget *widget, float x, float y)
{
  ZuiSplitView *sv = (ZuiSplitView *)widget;

  sv->hovered_handle = splitview_handle_at(sv, x, y);

  if (sv->dragging_handle < 0) return;

  int i = sv->dragging_handle;

  float new_pos;
  if (sv->vertical) {
    new_pos = y - widget->bounds.y - sv->drag_offset;
  } else {
    new_pos = x - widget->bounds.x - sv->drag_offset;
  }

  float total = sv->vertical ? widget->bounds.height : widget->bounds.width;
  size_t handle_count = sv->child_count - 1;
  float total_handle_space = (float)handle_count * sv->handle_size;
  float available = total - total_handle_space;

  float min_pos = sv->min_child_size;
  if (i > 0) {
    min_pos = sv->positions[i - 1] + sv->handle_size + sv->min_child_size;
  }

  float max_pos = available;
  for (size_t j = (size_t)i + 1; j < handle_count; j++) {
    max_pos -= sv->min_child_size;
  }
  max_pos -= sv->min_child_size;

  if (new_pos < min_pos) new_pos = min_pos;
  if (new_pos > max_pos) new_pos = max_pos;

  if (new_pos != sv->positions[i]) {
    sv->positions[i] = new_pos;
    widget->needs_layout = true;
  }
}

static void splitview_destroy(ZuiWidget *widget)
{
  ZuiSplitView *sv = (ZuiSplitView *)widget;
  for (size_t i = 0; i < sv->child_count; i++) {
    if (sv->children[i]) {
      zui_widget_destroy(sv->children[i]);
    }
  }
  free(sv->children);
  free(sv->positions);
}

static const ZuiWidgetVTable splitview_vtable = {
  .draw = splitview_draw,
  .layout = splitview_layout,
  .hit_test = splitview_hit_test,
  .hit_test_children = splitview_hit_test_children,
  .on_mouse_enter = splitview_on_mouse_enter,
  .on_mouse_leave = splitview_on_mouse_leave,
  .on_mouse_down = splitview_on_mouse_down,
  .on_mouse_up = splitview_on_mouse_up,
  .on_mouse_move = splitview_on_mouse_move,
  .destroy = splitview_destroy,
};

ZuiSplitView *zui_splitview_create(bool vertical)
{
  ZuiSplitView *sv = (ZuiSplitView *)zui_widget_create(
    sizeof(ZuiSplitView), ZUI_WIDGET_SPLITVIEW, &splitview_vtable);
  if (!sv) return NULL;

  sv->vertical = vertical;
  sv->children = NULL;
  sv->child_count = 0;
  sv->child_capacity = 0;
  sv->positions = NULL;
  sv->min_child_size = 50.0f;
  sv->handle_size = 6.0f;
  sv->dragging_handle = -1;
  sv->drag_offset = 0.0f;
  sv->hovered_handle = -1;
  sv->anchor = ZUI_SPLIT_ANCHOR_AUTO;
  sv->last_total = 0.0f;

  sv->grip_length = 40.0f;
  sv->handle_color = ZUI_COLOR(0.0f, 0.0f, 0.0f, 0.0f);
  sv->grip_color = ZUI_COLOR_HEX(0x555555);
  sv->grip_hover_color = ZUI_COLOR_HEX(0x777777);
  sv->grip_drag_color = ZUI_COLOR_HEX(0x4a9eff);

  sv->base.cursor = vertical ? ZUI_CURSOR_ROW_RESIZE : ZUI_CURSOR_COL_RESIZE;
  sv->base.preferred_size.width = 400.0f;
  sv->base.preferred_size.height = 300.0f;

  return sv;
}

void zui_splitview_set_vertical(ZuiSplitView *sv, bool vertical)
{
  if (!sv) return;
  sv->vertical = vertical;
  sv->base.cursor = vertical ? ZUI_CURSOR_ROW_RESIZE : ZUI_CURSOR_COL_RESIZE;
  sv->base.needs_layout = true;
}

void zui_splitview_add_child(ZuiSplitView *sv, ZuiWidget *widget)
{
  if (!sv || !widget) return;

  if (sv->child_count >= sv->child_capacity) {
    size_t new_cap = sv->child_capacity == 0 ? 4 : sv->child_capacity * 2;
    ZuiWidget **new_children = realloc(sv->children, new_cap * sizeof(ZuiWidget *));
    if (!new_children) return;
    sv->children = new_children;
    sv->child_capacity = new_cap;
  }

  widget->parent = (ZuiWidget *)sv;
  sv->children[sv->child_count] = widget;
  sv->child_count++;

  if (sv->child_count >= 2) {
    size_t handle_count = sv->child_count - 1;
    float *new_positions = realloc(sv->positions, handle_count * sizeof(float));
    if (new_positions) {
      sv->positions = new_positions;
      sv->positions[handle_count - 1] = 200.0f * (float)handle_count;
    }
  }

  sv->base.needs_layout = true;
}

size_t zui_splitview_get_child_count(ZuiSplitView *sv)
{
  if (!sv) return 0;
  return sv->child_count;
}

void zui_splitview_set_handle_position(ZuiSplitView *sv, size_t index, float position)
{
  if (!sv) return;
  if (sv->child_count < 2 || index >= sv->child_count - 1) return;
  sv->positions[index] = position;
  sv->base.needs_layout = true;
}

float zui_splitview_get_handle_position(ZuiSplitView *sv, size_t index)
{
  if (!sv) return 0.0f;
  if (sv->child_count < 2 || index >= sv->child_count - 1) return 0.0f;
  return sv->positions[index];
}

void zui_splitview_set_min_child_size(ZuiSplitView *sv, float min_size)
{
  if (!sv) return;
  sv->min_child_size = min_size;
  sv->base.needs_layout = true;
}

void zui_splitview_set_anchor(ZuiSplitView *sv, ZuiSplitAnchor anchor)
{
  if (!sv) return;
  sv->anchor = anchor;
}

void zui_splitview_set_handle_size(ZuiSplitView *sv, float size)
{
  if (!sv) return;
  sv->handle_size = size;
  sv->base.needs_layout = true;
}

void zui_splitview_set_grip_length(ZuiSplitView *sv, float length)
{
  if (!sv) return;
  sv->grip_length = length;
}

void zui_splitview_set_handle_color(ZuiSplitView *sv, ZuiColor color)
{
  if (!sv) return;
  sv->handle_color = color;
}

void zui_splitview_set_grip_color(ZuiSplitView *sv, ZuiColor normal,
                                   ZuiColor hover, ZuiColor drag)
{
  if (!sv) return;
  sv->grip_color = normal;
  sv->grip_hover_color = hover;
  sv->grip_drag_color = drag;
}

ZuiWidget *zui_splitview_as_widget(ZuiSplitView *sv)
{
  return (ZuiWidget *)sv;
}

static ZuiFont *g_default_font = NULL;

static ZuiFont *get_default_font(void)
{
  if (!g_default_font) {
    const char *font_paths[] = {
      "/usr/share/fonts/noto/NotoSans-Regular.ttf",
      "/usr/share/fonts/TTF/NotoSans-Regular.ttf",
      "/usr/share/fonts/TTF/DejaVuSans.ttf",
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
      "/usr/share/fonts/dejavu/DejaVuSans.ttf",
      "/usr/share/fonts/TTF/liberation/LiberationSans-Regular.ttf",
      "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
      "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
    };
    for (size_t i = 0; i < sizeof(font_paths) / sizeof(font_paths[0]); i++) {
      g_default_font = zui_font_load(font_paths[i], 16.0f);
      if (g_default_font) break;
    }
  }

  return g_default_font;
}

ZuiFont *zui_font_create(void)
{
  ZuiFont *font = NULL;
  const char *font_paths[] = {
    "/usr/share/fonts/noto/NotoSans-Regular.ttf",
    "/usr/share/fonts/TTF/NotoSans-Regular.ttf",
    "/usr/share/fonts/TTF/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/TTF/liberation/LiberationSans-Regular.ttf",
    "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
    "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
  };
  for (size_t i = 0; i < sizeof(font_paths) / sizeof(font_paths[0]); i++) {
    font = zui_font_load(font_paths[i], 16.0f);
    if (font) break;
  }
  return font;
}

ZuiLabel *zui_label_create(const char *text)
{
  ZuiLabel *label = (ZuiLabel *)zui_widget_create(
    sizeof(ZuiLabel), ZUI_WIDGET_LABEL, &label_vtable);
  if (!label) return NULL;

  label->text = text ? strdup(text) : NULL;
  label->text_color = ZUI_COLOR_HEX(0xffffff);
  label->font = get_default_font();
  label->owns_font = false;

  if (label->font && text) {
    label->base.preferred_size.width = zui_font_text_width(label->font, text) + 16;
    label->base.preferred_size.height = zui_font_text_height(label->font, text) + 8;
  } else {
    size_t len = text ? strlen(text) : 0;
    label->base.preferred_size.width = (float)(len * 8 + 16);
    label->base.preferred_size.height = 24.0f;
  }

  return label;
}

void zui_label_set_size(ZuiLabel *label, float size)
{
  if (!label) return;

  if (size < 9.0f) size = 9.0f;
  else if (size > 107.0f) size = 100.0f;

  if (label->font == g_default_font) label->font = zui_font_create();
  label->font->size = size;

  if (!init_font(label->font, label->font->font_data, size))
    free(label->font->font_data);

  if (label->font && label->text) {
    label->base.preferred_size.width = zui_font_text_width(label->font, label->text) + 16;
    label->base.preferred_size.height = zui_font_text_height(label->font, label->text) + 8;
  } else {
    size_t len = label->text ? strlen(label->text) : 0;
    label->base.preferred_size.width = (float)(len * 8 + 16);
    label->base.preferred_size.height = 24.0f;
  }
}

float zui_get_font_size(ZuiLabel *label)
{
  if (!label) return 0.0f;
  return label->font->size;
}

void zui_label_set_text(ZuiLabel *label, const char *text)
{
  if (!label) return;
  free(label->text);
  label->text = text ? strdup(text) : NULL;

  if (label->font && text) {
    label->base.preferred_size.width = zui_font_text_width(label->font, text) + 16;
    label->base.preferred_size.height = zui_font_text_height(label->font, text) + 8;
  } else {
    size_t len = text ? strlen(text) : 0;
    label->base.preferred_size.width = (float)(len * 8 + 16);
    label->base.preferred_size.height = 24.0f;
  }
}

void zui_label_set_font(ZuiLabel *label, ZuiFont *font)
{
  if (!label) return;
  if (label->owns_font && label->font) {
    zui_font_destroy(label->font);
  }
  label->font = font;
  label->owns_font = false;

  if (label->font && label->text) {
    label->base.preferred_size.width = zui_font_text_width(label->font, label->text) + 16;
    label->base.preferred_size.height = zui_font_text_height(label->font, label->text) + 8;
  }
}

void zui_label_set_color(ZuiLabel *label, ZuiColor color)
{
  if (!label) return;
  label->text_color = color;
}

static void close_button_click(ZuiWidget *widget, void *user_data)
{
  ZuiWindow *window = user_data;
  zui_window_close(window);
}

static void minimize_button_click(ZuiWidget *widget, void *user_data)
{
  (void)widget;
  ZuiWindow *window = user_data;
  zui_window_minimize(window);
}

static void hide_button_click(ZuiWidget *widget, void *user_data)
{
  (void)widget;
  ZuiWindow *window = user_data;
  zui_window_minimize(window);
}

static void maximize_button_click(ZuiWidget *widget, void *user_data)
{
  ZuiWindow *window = user_data;
  zui_window_maximize(window);
}

ZuiWidget *zui_window_close_button(ZuiWindow *window)
{
  ZuiIconButton *btn = create_icon_button("assets/icons/x-exit-icon.svg",
                                           12.0f, ZUI_COLOR_RGB(1.0f, 1.0f, 1.0f));
  if (!btn) return NULL;

  btn->base.normal_color = ZUI_COLOR_HEX(0xe81123);
  btn->base.hover_color = ZUI_COLOR_HEX(0xf1707a);
  btn->base.pressed_color = ZUI_COLOR_HEX(0xc42b1c);

  btn->base.base.preferred_size.width = 20.0f;
  btn->base.base.preferred_size.height = 20.0f;
  btn->base.base.corner_radius = 10.0f;

  zui_button_on_click(&btn->base, close_button_click, window);
  return (ZuiWidget *)btn;
}

ZuiWidget *zui_window_minimize_button(ZuiWindow *window)
{
  ZuiIconButton *btn = create_icon_button("assets/icons/x-minimize-icon.svg",
                                           12.0f, ZUI_COLOR_RGB(0.9f, 0.9f, 0.9f));
  if (!btn) return NULL;

  btn->base.normal_color = ZUI_COLOR_HEX(0x3d3d3d);
  btn->base.hover_color = ZUI_COLOR_HEX(0x4d4d4d);
  btn->base.pressed_color = ZUI_COLOR_HEX(0x2d2d2d);

  btn->base.base.preferred_size.width = 20.0f;
  btn->base.base.preferred_size.height = 20.0f;
  btn->base.base.corner_radius = 10.0f;

  zui_button_on_click(&btn->base, minimize_button_click, window);
  return (ZuiWidget *)btn;
}

ZuiWidget *zui_window_maximize_button(ZuiWindow *window)
{
  ZuiMaximizeButton *btn = (ZuiMaximizeButton *)zui_widget_create(
    sizeof(ZuiMaximizeButton), ZUI_WIDGET_BUTTON, &maximize_button_vtable);
  if (!btn) return NULL;

  btn->base.text = NULL;
  btn->base.normal_color = ZUI_COLOR_HEX(0x3d3d3d);
  btn->base.hover_color = ZUI_COLOR_HEX(0x4d4d4d);
  btn->base.pressed_color = ZUI_COLOR_HEX(0x2d2d2d);

  btn->base.base.preferred_size.width = 20.0f;
  btn->base.base.preferred_size.height = 20.0f;
  btn->base.base.corner_radius = 10.0f;
  btn->base.base.cursor = ZUI_CURSOR_POINTER;

  btn->icon_size = 12.0f;
  btn->icon_color = ZUI_COLOR_RGB(1.0f, 1.0f, 1.0f);
  btn->window = window;

  btn->maximize_icon = zui_icon_load_svg("assets/icons/x-maximize-icon.svg");
  btn->restore_icon = zui_icon_load_svg("assets/icons/x-minimize-icon.svg");

  if (btn->maximize_icon) {
    btn->maximize_texture = zui_texture_create(btn->maximize_icon->raster_data,
                                                btn->maximize_icon->raster_width,
                                                btn->maximize_icon->raster_height);
  }
  if (btn->restore_icon) {
    btn->restore_texture = zui_texture_create(btn->restore_icon->raster_data,
                                               btn->restore_icon->raster_width,
                                               btn->restore_icon->raster_height);
  }

  zui_button_on_click(&btn->base, maximize_button_click, window);
  return (ZuiWidget *)btn;
}

ZuiWidget *zui_window_hide_button(ZuiWindow *window)
{
  ZuiIconButton *btn = create_icon_button("assets/icons/x-hidden-icon.svg",
                                           12.0f, ZUI_COLOR_RGB(1.0f, 1.0f, 1.0f));
  if (!btn) return NULL;

  btn->base.normal_color = ZUI_COLOR_HEX(0x3d3d3d);
  btn->base.hover_color = ZUI_COLOR_HEX(0x4d4d4d);
  btn->base.pressed_color = ZUI_COLOR_HEX(0x2d2d2d);

  btn->base.base.preferred_size.width = 20.0f;
  btn->base.base.preferred_size.height = 20.0f;
  btn->base.base.corner_radius = 10.0f;

  zui_button_on_click(&btn->base, hide_button_click, window);
  return (ZuiWidget *)btn;
}
