#ifndef ZUI_WAYLAND_PLATFORM_H
#define ZUI_WAYLAND_PLATFORM_H

#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>
#include <stdbool.h>
#include <stdint.h>

#include "protocols/xdg-shell-client-protocol.h"
#include "protocols/xdg-decoration-unstable-v1-client-protocol.h"
#include <zui/cursor.h>

typedef struct ZuiWindow ZuiWindow;

typedef struct ZuiPlatform {
  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_compositor *compositor;
  struct wl_shm *shm;
  struct xdg_wm_base *xdg_wm_base;
  struct zxdg_decoration_manager_v1 *decoration_manager;
  struct wl_seat *seat;
  struct wl_pointer *pointer;
  struct wl_keyboard *keyboard;

  struct xkb_context *xkb_context;
  struct xkb_keymap *xkb_keymap;
  struct xkb_state *xkb_state;

  struct wl_cursor_theme *cursor_theme;
  struct wl_surface *cursor_surface;
  struct wl_cursor *cursors[ZUI_CURSOR_COUNT];
  ZuiCursor current_cursor;

  ZuiWindow *pointer_focus;
  ZuiWindow *keyboard_focus;

  double pointer_x;
  double pointer_y;
  uint32_t pointer_button;
  uint32_t pointer_serial;
  uint32_t pointer_enter_serial;
  uint32_t keyboard_serial;

  int32_t repeat_rate;
  int32_t repeat_delay;
  uint32_t repeat_key;
  uint32_t repeat_sym;
  char repeat_text[8];
  bool repeat_active;
  uint64_t repeat_next_time;

  bool running;
} ZuiPlatform;

typedef struct ZuiWaylandWindow {
  struct wl_surface *surface;
  struct xdg_surface *xdg_surface;
  struct xdg_toplevel *xdg_toplevel;
  struct zxdg_toplevel_decoration_v1 *decoration;
  struct wl_egl_window *egl_window;
  struct wl_callback *frame_callback;

  bool configured;
  bool pending_resize;
  int pending_width;
  int pending_height;
} ZuiWaylandWindow;

bool zui_platform_init(ZuiPlatform *platform);
void zui_platform_shutdown(ZuiPlatform *platform);
void zui_platform_poll_events(ZuiPlatform *platform);

bool zui_wayland_window_create(ZuiPlatform *platform, ZuiWaylandWindow *wl_win,
                                ZuiWindow *window, int width, int height,
                                const char *title);
void zui_wayland_window_destroy(ZuiPlatform *platform, ZuiWaylandWindow *wl_win);
void zui_wayland_window_set_title(ZuiWaylandWindow *wl_win, const char *title);
void zui_wayland_window_request_frame(ZuiWaylandWindow *wl_win, ZuiWindow *window);

void zui_wayland_start_move(ZuiPlatform *platform, ZuiWaylandWindow *wl_win);
void zui_wayland_start_resize(ZuiPlatform *platform, ZuiWaylandWindow *wl_win,
                               uint32_t edges);
void zui_wayland_minimize(ZuiWaylandWindow *wl_win);
void zui_wayland_maximize(ZuiWaylandWindow *wl_win);
void zui_wayland_unmaximize(ZuiWaylandWindow *wl_win);
void zui_wayland_set_min_size(ZuiWaylandWindow *wl_win, int width, int height);
void zui_wayland_set_max_size(ZuiWaylandWindow *wl_win, int width, int height);
void zui_wayland_show_window_menu(ZuiPlatform *platform, ZuiWaylandWindow *wl_win,
                                   int x, int y);
void zui_wayland_set_fullscreen(ZuiWaylandWindow *wl_win, bool fullscreen);
void zui_wayland_set_decorated(ZuiPlatform *platform, ZuiWaylandWindow *wl_win,
                                bool decorated);

void zui_platform_set_cursor(ZuiPlatform *platform, ZuiCursor cursor);

bool zui_platform_get_ctrl(ZuiPlatform *platform);
bool zui_platform_get_shift(ZuiPlatform *platform);
bool zui_platform_get_alt(ZuiPlatform *platform);

#endif
