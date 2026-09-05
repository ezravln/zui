#ifndef ZUI_WIDGET_H
#define ZUI_WIDGET_H

#include <stdbool.h>
#include <stddef.h>
#include "color.h"
#include "font.h"
#include "layout.h"

typedef struct ZuiWidget ZuiWidget;
typedef struct ZuiButton ZuiButton;
typedef struct ZuiLabel ZuiLabel;
typedef struct ZuiPanel ZuiPanel;
typedef struct ZuiCheckbox ZuiCheckbox;
typedef struct ZuiScrollView ZuiScrollView;
typedef struct ZuiScroller ZuiScroller;
typedef struct ZuiSplitView ZuiSplitView;
typedef struct ZuiRadioGroup ZuiRadioGroup;
typedef struct ZuiRadioButton ZuiRadioButton;
typedef struct ZuiTextInput ZuiTextInput;

typedef void (*ZuiCheckboxCallback)(ZuiCheckbox *checkbox, bool checked,
                                     void *user_data);
typedef void (*ZuiScrollerCallback)(ZuiScroller *scroller, float value,
                                     void *user_data);
typedef void (*ZuiRadioCallback)(ZuiRadioGroup *group, ZuiRadioButton *selected,
                                  void *user_data);
typedef void (*ZuiTextInputCallback)(ZuiTextInput *input, const char *text,
                                      void *user_data);

typedef void (*ZuiClickCallback)(ZuiWidget *widget, void *user_data);

typedef enum ZuiCursor {
  ZUI_CURSOR_DEFAULT,
  ZUI_CURSOR_POINTER,
  ZUI_CURSOR_TEXT,
  ZUI_CURSOR_CROSSHAIR,
  ZUI_CURSOR_MOVE,
  ZUI_CURSOR_NOT_ALLOWED,
  ZUI_CURSOR_GRAB,
  ZUI_CURSOR_GRABBING,
  ZUI_CURSOR_COL_RESIZE,
  ZUI_CURSOR_ROW_RESIZE,
} ZuiCursor;

void zui_widget_destroy(ZuiWidget *widget);
void zui_widget_add_child(ZuiWidget *parent, ZuiWidget *child);
void zui_widget_focus(ZuiWidget *widget);
void zui_widget_unfocus(ZuiWidget *widget);

void zui_set_visible(ZuiWidget *widget, bool visible);
void zui_set_background(ZuiWidget *widget, ZuiColor color);
void zui_set_corner_radius(ZuiWidget *widget, float radius);
void zui_set_cursor(ZuiWidget *widget, ZuiCursor cursor);
void zui_set_fill(ZuiWidget *widget, bool fill_width, bool fill_height);
void zui_set_size(ZuiWidget *widget, float width, float height);
void zui_set_padding(ZuiWidget *widget, float padding);
void zui_set_spacing(ZuiWidget *widget, float spacing);

#define zui_widget_set_visible zui_set_visible
#define zui_widget_set_background zui_set_background
#define zui_widget_set_corner_radius zui_set_corner_radius
#define zui_widget_set_cursor zui_set_cursor
#define zui_widget_set_fill zui_set_fill

ZuiButton *zui_button_create(const char *text);
void zui_button_set_text(ZuiButton *button, const char *text);
void zui_button_set_size(ZuiButton *button, float width, float height);
void zui_button_set_color(ZuiButton *button, ZuiColor color);
void zui_button_on_click(ZuiButton *button, ZuiClickCallback callback,
                          void *user_data);
void zui_button_set_icon(ZuiButton *button, const char *image_path, float size);
void zui_button_set_text_color(ZuiButton *button, ZuiColor color);

ZuiLabel *zui_label_create(const char *text);
void zui_label_set_size(ZuiLabel *label, float size);
void zui_label_set_text(ZuiLabel *label, const char *text);
void zui_label_set_font(ZuiLabel *label, ZuiFont *font);
void zui_label_set_color(ZuiLabel *label, ZuiColor color);

float zui_get_font_size(ZuiLabel *label);

ZuiCheckbox *zui_checkbox_create(const char *label);
void zui_checkbox_set_checked(ZuiCheckbox *checkbox, bool checked);
bool zui_checkbox_is_checked(ZuiCheckbox *checkbox);
void zui_checkbox_set_label(ZuiCheckbox *checkbox, const char *label);
void zui_checkbox_set_size(ZuiCheckbox *checkbox, float size);
void zui_checkbox_set_box_color(ZuiCheckbox *checkbox, ZuiColor unchecked,
                                 ZuiColor checked);
void zui_checkbox_set_icon_color(ZuiCheckbox *checkbox, ZuiColor color);
void zui_checkbox_set_icon(ZuiCheckbox *checkbox, const char *svg_path);
void zui_checkbox_on_change(ZuiCheckbox *checkbox, ZuiCheckboxCallback callback,
                             void *user_data);

ZuiRadioGroup *zui_radiogroup_create(void);
void zui_radiogroup_destroy(ZuiRadioGroup *group);
void zui_radiogroup_on_change(ZuiRadioGroup *group, ZuiRadioCallback callback,
                               void *user_data);
ZuiRadioButton *zui_radiogroup_get_selected(ZuiRadioGroup *group);
int zui_radiogroup_get_selected_index(ZuiRadioGroup *group);
void zui_radiogroup_select(ZuiRadioGroup *group, size_t index);

ZuiRadioButton *zui_radiobutton_create(ZuiRadioGroup *group, const char *label);
void zui_radiobutton_set_colors(ZuiRadioButton *rb, ZuiColor circle,
                                 ZuiColor selected, ZuiColor dot);
void zui_radiobutton_set_label_color(ZuiRadioButton *rb, ZuiColor color);
void zui_radiobutton_set_size(ZuiRadioButton *rb, float size);
const char *zui_radiobutton_get_label(ZuiRadioButton *rb);
ZuiWidget *zui_radiobutton_as_widget(ZuiRadioButton *rb);

ZuiPanel *zui_panel_create(void);
void zui_panel_destroy(ZuiPanel *panel);
void zui_panel_set_layout(ZuiPanel *panel, ZuiLayoutDir direction);
void zui_panel_set_padding(ZuiPanel *panel, float top, float right, float bottom, float left);
void zui_panel_set_spacing(ZuiPanel *panel, float spacing);
void zui_panel_set_alignment(ZuiPanel *panel, ZuiAlign main_axis, ZuiAlign cross_axis);
void zui_panel_set_background(ZuiPanel *panel, ZuiColor color);
void zui_panel_set_corner_radius(ZuiPanel *panel, float radius);
void zui_panel_set_size(ZuiPanel *panel, float width, float height);
void zui_panel_set_fill(ZuiPanel *panel, bool fill_width, bool fill_height);
void zui_panel_add_child(ZuiPanel *panel, ZuiWidget *child);
void zui_panel_remove_child(ZuiPanel *panel, ZuiWidget *child);
void zui_panel_clear(ZuiPanel *panel);
ZuiWidget *zui_panel_as_widget(ZuiPanel *panel);

typedef void (*ZuiScrollViewCallback)(ZuiScrollView *sv, float x, float y,
                                       void *user_data);

ZuiScrollView *zui_scrollview_create(void);
void zui_scrollview_set_content(ZuiScrollView *sv, ZuiWidget *content);
void zui_scrollview_set_size(ZuiScrollView *sv, float width, float height);
void zui_scrollview_scroll_to(ZuiScrollView *sv, float x, float y);
void zui_scrollview_get_scroll(ZuiScrollView *sv, float *x, float *y);
void zui_scrollview_on_scroll(ZuiScrollView *sv, ZuiScrollViewCallback callback,
                               void *user_data);
ZuiWidget *zui_scrollview_as_widget(ZuiScrollView *sv);

ZuiScroller *zui_scroller_create(bool vertical);
void zui_scroller_set_size(ZuiScroller *scroller, float width, float height);
void zui_scroller_set_range(ZuiScroller *scroller, float content_size,
                             float viewport_size);
void zui_scroller_set_value(ZuiScroller *scroller, float value);
float zui_scroller_get_value(ZuiScroller *scroller);
void zui_scroller_on_change(ZuiScroller *scroller, ZuiScrollerCallback callback,
                             void *user_data);
ZuiWidget *zui_scroller_as_widget(ZuiScroller *scroller);

typedef enum ZuiSplitAnchor {
  ZUI_SPLIT_ANCHOR_AUTO,
  ZUI_SPLIT_ANCHOR_START,
  ZUI_SPLIT_ANCHOR_CENTER,
  ZUI_SPLIT_ANCHOR_END,
} ZuiSplitAnchor;

ZuiSplitView *zui_splitview_create(bool vertical);
void zui_splitview_set_vertical(ZuiSplitView *sv, bool vertical);
void zui_splitview_add_child(ZuiSplitView *sv, ZuiWidget *widget);
size_t zui_splitview_get_child_count(ZuiSplitView *sv);
void zui_splitview_set_handle_position(ZuiSplitView *sv, size_t index, float position);
float zui_splitview_get_handle_position(ZuiSplitView *sv, size_t index);
void zui_splitview_set_min_child_size(ZuiSplitView *sv, float min_size);
void zui_splitview_set_anchor(ZuiSplitView *sv, ZuiSplitAnchor anchor);
void zui_splitview_set_handle_size(ZuiSplitView *sv, float size);
void zui_splitview_set_grip_length(ZuiSplitView *sv, float length);
void zui_splitview_set_handle_color(ZuiSplitView *sv, ZuiColor color);
void zui_splitview_set_grip_color(ZuiSplitView *sv, ZuiColor normal,
                                   ZuiColor hover, ZuiColor drag);
ZuiWidget *zui_splitview_as_widget(ZuiSplitView *sv);

ZuiTextInput *zui_textinput_create(const char *placeholder);
void zui_textinput_set_text(ZuiTextInput *input, const char *text);
const char *zui_textinput_get_text(ZuiTextInput *input);
void zui_textinput_set_placeholder(ZuiTextInput *input, const char *placeholder);
void zui_textinput_set_size(ZuiTextInput *input, float width, float height);
void zui_textinput_set_colors(ZuiTextInput *input, ZuiColor background,
                               ZuiColor text, ZuiColor placeholder,
                               ZuiColor cursor, ZuiColor border);
void zui_textinput_on_change(ZuiTextInput *input, ZuiTextInputCallback callback,
                              void *user_data);
ZuiWidget *zui_textinput_as_widget(ZuiTextInput *input);

#endif
