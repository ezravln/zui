#ifndef ZUI_STRUCTS_H
#define ZUI_STRUCTS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "color.h"
#include "layout.h"
#include "widget.h"
#include "internal/texture.h"

typedef struct ZuiRenderer ZuiRenderer;
typedef struct ZuiWidget ZuiWidget;
typedef struct ZuiWindow ZuiWindow;
typedef struct ZuiWindowDecoration ZuiWindowDecoration;
typedef struct ZuiButton ZuiButton;
typedef struct ZuiLabel ZuiLabel;
typedef struct ZuiContainer ZuiContainer;
typedef struct ZuiPanel ZuiPanel;
typedef struct ZuiCheckbox ZuiCheckbox;
typedef struct ZuiScrollView ZuiScrollView;
typedef struct ZuiScroller ZuiScroller;
typedef struct ZuiSplitView ZuiSplitView;
typedef struct ZuiRadioGroup ZuiRadioGroup;
typedef struct ZuiRadioButton ZuiRadioButton;
typedef struct ZuiTextInput ZuiTextInput;
typedef struct ZuiSlider ZuiSlider;
typedef struct ZuiDropdown ZuiDropdown;
typedef struct ZuiProgressBar ZuiProgressBar;
typedef struct ZuiGridView ZuiGridView;
typedef struct ZuiMenuBar ZuiMenuBar;
typedef struct ZuiMenu ZuiMenu;
typedef struct ZuiMenuItem ZuiMenuItem;
typedef struct ZuiPieChart ZuiPieChart;
typedef struct ZuiBarChart ZuiBarChart;
typedef struct ZuiLineChart ZuiLineChart;
typedef struct ZuiCircularProgress ZuiCircularProgress;
typedef struct ZuiVideo ZuiVideo;
typedef struct ZuiTabView ZuiTabView;
typedef struct ZuiTreeView ZuiTreeView;
typedef struct ZuiTreeNode ZuiTreeNode;
typedef struct ZuiTextArea ZuiTextArea;
typedef struct ZuiTextSpan ZuiTextSpan;
typedef struct ZuiIcon ZuiIcon;

typedef struct ZuiFont ZuiFont;

typedef struct ZuiPoint {
  float x, y;
} ZuiPoint;

typedef struct ZuiSize {
  float width, height;
} ZuiSize;

typedef struct ZuiBounds {
  float x, y, width, height;
} ZuiBounds;

typedef struct ZuiPadding {
  float top, right, bottom, left;
} ZuiPadding;

#define ZUI_PADDING(t, r, b, l) ((ZuiPadding){(t), (r), (b), (l)})
#define ZUI_PADDING_ALL(v) ((ZuiPadding){(v), (v), (v), (v)})
#define ZUI_PADDING_XY(x, y) ((ZuiPadding){(y), (x), (y), (x)})

typedef struct ZuiRadius {
  float tl, tr, br, bl;
} ZuiRadius;

#define ZUI_RADIUS(tl, tr, br, bl) ((ZuiRadius){(tl), (tr), (br), (bl)})
#define ZUI_RADIUS_ALL(v) ((ZuiRadius){(v), (v), (v), (v)})
#define ZUI_RADIUS_TOP(v) ((ZuiRadius){(v), (v), 0, 0})
#define ZUI_RADIUS_BOTTOM(v) ((ZuiRadius){0, 0, (v), (v)})


typedef enum ZuiTabPosition {
  ZUI_TAB_POSITION_TOP,
  ZUI_TAB_POSITION_BOTTOM,
  ZUI_TAB_POSITION_LEFT,
  ZUI_TAB_POSITION_RIGHT,
} ZuiTabPosition;

typedef void (*ZuiClickCallback)(ZuiWidget *widget, void *user_data);
typedef void (*ZuiDrawCallback)(ZuiWidget *widget, void *renderer);
typedef void (*ZuiCheckboxCallback)(ZuiCheckbox *checkbox, bool checked,
                                     void *user_data);
typedef void (*ZuiScrollerCallback)(ZuiScroller *scroller, float value,
                                     void *user_data);
typedef void (*ZuiRadioCallback)(ZuiRadioGroup *group, ZuiRadioButton *selected,
                                  void *user_data);
typedef void (*ZuiTextInputCallback)(ZuiTextInput *input, const char *text,
                                      void *user_data);
typedef void (*ZuiSliderCallback)(ZuiSlider *slider, float value,
                                   void *user_data);
typedef void (*ZuiDropdownCallback)(ZuiDropdown *dropdown, int index,
                                     const char *item, void *user_data);
typedef void (*ZuiMenuItemCallback)(ZuiMenuItem *item, void *user_data);
typedef void (*ZuiScrollViewCallback)(ZuiScrollView *sv, float x, float y,
                                       void *user_data);
typedef struct ZuiIconSource ZuiIconSource;

#define ZUI_MAX_CHILDREN 64

typedef enum ZuiCornerMode {
  ZUI_CORNERS_ALL,
  ZUI_CORNERS_TOP,
  ZUI_CORNERS_BOTTOM,
  ZUI_CORNERS_NONE,
} ZuiCornerMode;

typedef struct ZuiWidgetVTable {
  void (*draw)(ZuiWidget *widget, ZuiRenderer *renderer);
  void (*draw_overlay)(ZuiWidget *widget, ZuiRenderer *renderer);
  void (*layout)(ZuiWidget *widget);
  void (*destroy)(ZuiWidget *widget);
  bool (*hit_test)(ZuiWidget *widget, float x, float y);
  ZuiWidget *(*hit_test_children)(ZuiWidget *widget, float x, float y);
  void (*on_mouse_enter)(ZuiWidget *widget);
  void (*on_mouse_leave)(ZuiWidget *widget);
  void (*on_mouse_down)(ZuiWidget *widget, float x, float y, uint32_t button);
  void (*on_mouse_up)(ZuiWidget *widget, float x, float y, uint32_t button);
  void (*on_mouse_move)(ZuiWidget *widget, float x, float y);
  void (*on_scroll)(ZuiWidget *widget, double dx, double dy);
  void (*on_key)(ZuiWidget *widget, uint32_t key, uint32_t sym,
                  const char *text, bool pressed);
  void (*on_focus)(ZuiWidget *widget, bool focused);
} ZuiWidgetVTable;

struct ZuiWidget {
  ZuiWidgetType type;
  const ZuiWidgetVTable *vtable;

  ZuiBounds bounds;
  ZuiSize min_size;
  ZuiSize preferred_size;
  bool visible;
  bool hovered;
  bool pressed;

  ZuiWidget *parent;
  ZuiWidget *children[ZUI_MAX_CHILDREN];
  int child_count;

  ZuiColor background;
  float corner_radius;
  ZuiCornerMode corner_mode;
  float padding;
  float spacing;
  ZuiLayoutDir layout_dir;
  ZuiAlign align;
  bool expand;
  bool fill_width;
  bool fill_height;
  bool needs_layout;
  ZuiCursor cursor;

  ZuiClickCallback on_click;
  void *user_data;
};

typedef struct ZuiEdgeInsets {
  float top;
  float right;
  float bottom;
  float left;
} ZuiEdgeInsets;

#define ZUI_INSETS(t, r, b, l) ((ZuiEdgeInsets){(t), (r), (b), (l)})
#define ZUI_INSETS_ALL(v) ((ZuiEdgeInsets){(v), (v), (v), (v)})
#define ZUI_INSETS_XY(x, y) ((ZuiEdgeInsets){(y), (x), (y), (x)})

struct ZuiPanel {
  ZuiWidget base;
  ZuiLayoutDir layout_dir;
  ZuiEdgeInsets padding;
  float spacing;
  ZuiAlign main_align;
  ZuiAlign cross_align;
};

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

struct ZuiLabel {
  ZuiWidget base;
  char *text;
  ZuiColor text_color;
  ZuiFont *font;
  bool owns_font;
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

struct ZuiProgressBar {
  ZuiWidget base;
  float value;
  ZuiColor track_color;
  ZuiColor fill_color;
  float corner_radius;
};

struct ZuiCircularProgress {
  ZuiWidget base;
  float value;
  float thickness;
  ZuiColor track_color;
  ZuiColor fill_color;
  char *text;
  char *subtitle;
  bool show_percentage;
  ZuiColor text_color;
  ZuiColor subtitle_color;
  ZuiFont *font;
  ZuiFont *subtitle_font;
};

struct ZuiSlider {
  ZuiWidget base;
  float min_value;
  float max_value;
  float value;
  bool dragging;
  ZuiColor track_color;
  ZuiColor fill_color;
  ZuiColor thumb_color;
  ZuiColor thumb_hover_color;
  ZuiColor thumb_drag_color;
  float thumb_radius;
  ZuiSliderCallback on_change;
  void *change_user_data;
};

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

#define ZUI_DROPDOWN_MAX_ITEMS 64

struct ZuiDropdown {
  ZuiWidget base;
  char *placeholder;
  char *items[ZUI_DROPDOWN_MAX_ITEMS];
  size_t item_count;
  int selected_index;
  bool open;
  int hover_index;
  ZuiFont *font;
  ZuiColor bg_color;
  ZuiColor text_color;
  ZuiColor placeholder_color;
  ZuiColor border_color;
  ZuiColor hover_color;
  ZuiColor item_bg_color;
  float corner_radius;
  float padding;
  float item_height;
  ZuiDropdownCallback on_change;
  void *change_user_data;
  ZuiIconSource *chevron_down_icon;
  ZuiIconSource *chevron_up_icon;
  ZuiTexture chevron_down_texture;
  ZuiTexture chevron_up_texture;
  float icon_size;
};

#define ZUI_GRIDVIEW_MAX_CHILDREN 256

struct ZuiGridView {
  ZuiWidget base;
  ZuiWidget *children[ZUI_GRIDVIEW_MAX_CHILDREN];
  size_t child_count;
  int columns;
  float cell_width;
  float cell_height;
  float gap_x;
  float gap_y;
  float padding;
  float scroll_x;
  float scroll_y;
  float content_width;
  float content_height;
  ZuiColor bg_color;
};

#define ZUI_MENU_MAX_ITEMS 32

struct ZuiMenuItem {
  ZuiWidget base;
  char *label;
  char *shortcut;
  bool enabled;
  bool is_separator;
  ZuiFont *font;
  ZuiColor text_color;
  ZuiColor hover_color;
  ZuiColor disabled_color;
  ZuiMenuItemCallback on_click_cb;
  void *click_user_data;
  ZuiIconSource *icon_source;
  ZuiTexture icon_texture;
  float icon_size;
};

struct ZuiMenu {
  ZuiWidget base;
  char *title;
  ZuiMenuItem *items[ZUI_MENU_MAX_ITEMS];
  size_t item_count;
  bool open;
  int hover_index;
  ZuiFont *font;
  ZuiColor text_color;
  ZuiColor bg_color;
  ZuiColor hover_bg_color;
  ZuiColor popup_bg_color;
  ZuiColor border_color;
  float popup_width;
};

struct ZuiMenuBar {
  ZuiWidget base;
  ZuiMenu **menus;
  size_t menu_count;
  size_t menu_capacity;
  ZuiMenu *active_menu;
  ZuiColor bg_color;
  ZuiColor text_color;
  ZuiFont *font;
};

typedef void (*ZuiPieChartCallback)(ZuiPieChart *chart, int slice_index,
                                     void *user_data);
typedef void (*ZuiBarChartCallback)(ZuiBarChart *chart, int bar_index,
                                     void *user_data);

struct ZuiPieChart {
  ZuiWidget base;
  float *values;
  ZuiColor *colors;
  char **labels;
  size_t slice_count;
  size_t slice_capacity;
  float hole_radius;
  bool show_labels;
  bool show_values;
  int hovered_slice;
  ZuiPieChartCallback hover_callback;
  void *hover_user_data;
  ZuiPieChartCallback click_callback;
  void *click_user_data;
  ZuiFont *font;
};

struct ZuiBarChart {
  ZuiWidget base;
  float *values;
  ZuiColor *colors;
  char **labels;
  size_t bar_count;
  size_t bar_capacity;
  float max_value;
  float bar_spacing;
  float corner_radius;
  bool show_labels;
  bool show_values;
  int hovered_bar;
  ZuiBarChartCallback hover_callback;
  void *hover_user_data;
  ZuiBarChartCallback click_callback;
  void *click_user_data;
  ZuiFont *font;
};

struct ZuiLineChart {
  ZuiWidget base;
  float *values;
  size_t point_count;
  size_t point_capacity;
  float max_value;
  ZuiColor line_color;
  float line_thickness;
  bool show_points;
};

typedef void (*ZuiTabCallback)(ZuiTabView *tabview, int index, void *user_data);

struct ZuiTabView {
  ZuiWidget base;
  char **titles;
  ZuiWidget **contents;
  size_t tab_count;
  size_t tab_capacity;
  int selected_index;
  float tab_height;
  ZuiTabPosition tab_position;
  ZuiColor bg_color;
  ZuiColor tab_inactive_color;
  ZuiColor tab_active_color;
  ZuiColor text_color;
  ZuiFont *font;
  ZuiTabCallback on_change;
  void *change_user_data;
};

struct ZuiTreeNode {
  char *label;
  char *icon_path;
  void *user_data;
  ZuiTreeNode *parent;
  ZuiTreeNode **children;
  size_t child_count;
  size_t child_capacity;
  bool expanded;
  int depth;
};

typedef void (*ZuiTreeNodeCallback)(ZuiTreeView *tree, ZuiTreeNode *node,
                                     void *user_data);

struct ZuiTreeView {
  ZuiWidget base;
  ZuiTreeNode *root;
  ZuiTreeNode *selected;
  ZuiTreeNode *hovered;
  float indent;
  float row_height;
  float scroll_y;
  ZuiColor bg_color;
  ZuiColor text_color;
  ZuiColor selected_color;
  ZuiColor hover_color;
  ZuiFont *font;
  ZuiTreeNodeCallback on_select;
  void *select_user_data;
  ZuiTreeNodeCallback on_expand;
  void *expand_user_data;
};

#define ZUI_TEXTAREA_INITIAL_CAPACITY 4096

typedef struct ZuiTextHighlight {
  size_t start;
  size_t end;
  ZuiColor color;
  bool is_background;
} ZuiTextHighlight;

typedef void (*ZuiTextAreaCallback)(ZuiTextArea *textarea, const char *text,
                                     void *user_data);

struct ZuiTextArea {
  ZuiWidget base;
  char *text;
  size_t text_capacity;
  size_t text_len;
  char *placeholder;
  size_t cursor_pos;
  size_t selection_start;
  size_t selection_end;
  bool focused;
  bool dragging;
  bool readonly;
  bool show_line_numbers;
  ZuiFont *font;
  ZuiColor bg_color;
  ZuiColor text_color;
  ZuiColor placeholder_color;
  ZuiColor cursor_color;
  ZuiColor border_color;
  ZuiColor selection_color;
  ZuiColor line_num_bg_color;
  ZuiColor line_num_text_color;
  float corner_radius;
  float padding;
  float scroll_x;
  float scroll_y;
  ZuiTextHighlight *highlights;
  size_t highlight_count;
  size_t highlight_capacity;
  ZuiTextAreaCallback on_change;
  void *change_user_data;
};

#endif
