#ifndef ZUI_WINDOW_H
#define ZUI_WINDOW_H

#include <stdbool.h>
#include <stddef.h>
#include <zui/color.h>

typedef struct ZuiWindow ZuiWindow;
typedef struct ZuiWidget ZuiWidget;
typedef struct ZuiWindowDecoration ZuiWindowDecoration;

ZuiWindow *zui_window_create(int width, int height, const char *title);
void zui_window_destroy(ZuiWindow *window);
void zui_window_show(ZuiWindow *window);
bool zui_window_running(ZuiWindow *window);
void zui_window_render(ZuiWindow *window);

void zui_window_set_title(ZuiWindow *window, const char *title);
void zui_window_set_corner_radius(ZuiWindow *window, float radius);
void zui_window_set_min_size(ZuiWindow *window, int width, int height);
void zui_window_set_max_size(ZuiWindow *window, int width, int height);

ZuiWidget *zui_window_content(ZuiWindow *window);

void zui_window_minimize(ZuiWindow *window);
void zui_window_maximize(ZuiWindow *window);
void zui_window_hide(ZuiWindow *window);
void zui_window_close(ZuiWindow *window);

bool zui_window_is_maximized(ZuiWindow *window);
bool zui_window_is_minimized(ZuiWindow *window);
bool zui_window_is_hidden(ZuiWindow *window);

void zui_window_set_background_color(ZuiWindow *window, ZuiColor color);
void zui_window_set_border_color(ZuiWindow *window, ZuiColor color);
void zui_window_get_size(ZuiWindow *window, int *width, int *height);
void zui_window_set_position(ZuiWindow *window, int x, int y);
void zui_window_get_position(ZuiWindow *window, int *x, int *y);
bool zui_window_is_active(ZuiWindow *window);

void zui_window_show_menu(ZuiWindow *window, int x, int y);
void zui_window_set_fullscreen(ZuiWindow *window, bool fullscreen);
bool zui_window_is_fullscreen(ZuiWindow *window);
void zui_window_set_decorated(ZuiWindow *window, bool decorated);
bool zui_window_is_decorated(ZuiWindow *window);

void zui_add_window_moving_handler(ZuiWindow *window, ZuiWidget *widget);
void zui_remove_window_moving_handler(ZuiWindow *window, ZuiWidget *widget);

void zui_add_close_window_handler(ZuiWindow *window, ZuiWidget *widget);
void zui_remove_close_window_handler(ZuiWindow *window, ZuiWidget *widget);

void zui_add_window_maximize_handler(ZuiWindow *window, ZuiWidget *widget);
void zui_remove_window_maximize_handler(ZuiWindow *window, ZuiWidget *widget);

void zui_add_hide_window_handler(ZuiWindow *window, ZuiWidget *widget);
void zui_remove_hide_window_handler(ZuiWindow *window, ZuiWidget *widget);

void zui_add_minimize_window_handler(ZuiWindow *window, ZuiWidget *widget);
void zui_remove_minimize_window_handler(ZuiWindow *window, ZuiWidget *widget);

void zui_set_window_decoration(ZuiWindow *window, ZuiWidget *decoration);
ZuiWidget *zui_get_window_decoration(ZuiWindow *window);
void zui_window_decoration_set_visible(ZuiWindow *window, bool visible);
bool zui_window_decoration_is_visible(ZuiWindow *window);

ZuiWindowDecoration *zui_default_window_decoration(ZuiWindow *window);
void zui_window_decoration_destroy(ZuiWindowDecoration *decor);
void zui_window_decoration_set_title(ZuiWindowDecoration *decor, const char *title);
void zui_window_decoration_set_logo(ZuiWindowDecoration *decor, const char *path);
void zui_window_decoration_set_logo_from_memory(ZuiWindowDecoration *decor,
                                                 const unsigned char *data,
                                                 size_t size);
ZuiWidget *zui_window_decoration_widget(ZuiWindowDecoration *decor);

typedef void (*ZuiCustomDrawCallback)(ZuiWindow *window, void *user_data);
void zui_window_set_custom_draw(ZuiWindow *window, ZuiCustomDrawCallback callback, void *user_data);

#endif
