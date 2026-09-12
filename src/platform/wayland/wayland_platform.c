#include <zui/internal/wayland_platform.h>
#include <zui/internal/window_internal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/mman.h>
#include <time.h>
#include <linux/input-event-codes.h>

static uint64_t get_time_ms(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static char g_cursor_theme[256] = {0};
static int g_cursor_size = 0;

static void read_gsettings_cursor(void)
{
  FILE *p;
  char buf[64];

  if (g_cursor_size <= 0) {
    p = popen("gsettings get org.gnome.desktop.interface cursor-size 2>/dev/null", "r");
    if (p) {
      if (fgets(buf, sizeof(buf), p)) {
        int size = atoi(buf);
        if (size > 0) g_cursor_size = size;
      }
      pclose(p);
    }
  }

  if (!g_cursor_theme[0]) {
    p = popen("gsettings get org.gnome.desktop.interface cursor-theme 2>/dev/null", "r");
    if (p) {
      if (fgets(buf, sizeof(buf), p)) {
        char *start = strchr(buf, '\'');
        if (start) {
          start++;
          char *end = strchr(start, '\'');
          if (end) {
            *end = 0;
            strncpy(g_cursor_theme, start, sizeof(g_cursor_theme) - 1);
          }
        }
      }
      pclose(p);
    }
  }
}

static void read_cursor_config(void)
{
  const char *env_theme = getenv("XCURSOR_THEME");
  const char *env_size = getenv("XCURSOR_SIZE");

  if (env_theme && env_theme[0]) {
    strncpy(g_cursor_theme, env_theme, sizeof(g_cursor_theme) - 1);
  }
  if (env_size && env_size[0]) {
    g_cursor_size = atoi(env_size);
  }

  if (g_cursor_theme[0] && g_cursor_size > 0) return;

  const char *home = getenv("HOME");
  if (!home) return;

  char path[512];
  FILE *f;
  char line[256];

  snprintf(path, sizeof(path), "%s/.config/kcminputrc", home);
  f = fopen(path, "r");
  if (f) {
    while (fgets(line, sizeof(line), f)) {
      if (!g_cursor_theme[0] && strncmp(line, "cursorTheme=", 12) == 0) {
        char *val = line + 12;
        val[strcspn(val, "\r\n")] = 0;
        strncpy(g_cursor_theme, val, sizeof(g_cursor_theme) - 1);
      }
      if (g_cursor_size <= 0 && strncmp(line, "cursorSize=", 11) == 0) {
        g_cursor_size = atoi(line + 11);
      }
    }
    fclose(f);
  }

  if (g_cursor_theme[0] && g_cursor_size > 0) return;

  snprintf(path, sizeof(path), "%s/.config/gtk-3.0/settings.ini", home);
  f = fopen(path, "r");
  if (f) {
    while (fgets(line, sizeof(line), f)) {
      if (!g_cursor_theme[0] && strncmp(line, "gtk-cursor-theme-name=", 22) == 0) {
        char *val = line + 22;
        val[strcspn(val, "\r\n")] = 0;
        strncpy(g_cursor_theme, val, sizeof(g_cursor_theme) - 1);
      }
      if (g_cursor_size <= 0 && strncmp(line, "gtk-cursor-theme-size=", 22) == 0) {
        g_cursor_size = atoi(line + 22);
      }
    }
    fclose(f);
  }

  if (g_cursor_theme[0] && g_cursor_size > 0) return;

  read_gsettings_cursor();

  if (g_cursor_size <= 0) g_cursor_size = 24;
}

static void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base,
                              uint32_t serial)
{
  xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
  .ping = xdg_wm_base_ping,
};

static void pointer_enter(void *data, struct wl_pointer *pointer,
                          uint32_t serial, struct wl_surface *surface,
                          wl_fixed_t sx, wl_fixed_t sy)
{
  ZuiPlatform *platform = data;
  platform->pointer_serial = serial;
  platform->pointer_enter_serial = serial;
  platform->pointer_x = wl_fixed_to_double(sx);
  platform->pointer_y = wl_fixed_to_double(sy);

  ZuiWindow *window = wl_surface_get_user_data(surface);
  platform->pointer_focus = window;

  platform->current_cursor = (ZuiCursor)-1;
  zui_platform_set_cursor(platform, ZUI_CURSOR_DEFAULT);
}

static void pointer_leave(void *data, struct wl_pointer *pointer,
                          uint32_t serial, struct wl_surface *surface)
{
  ZuiPlatform *platform = data;
  platform->pointer_focus = NULL;
}

static void pointer_motion(void *data, struct wl_pointer *pointer,
                           uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
  ZuiPlatform *platform = data;
  platform->pointer_x = wl_fixed_to_double(sx);
  platform->pointer_y = wl_fixed_to_double(sy);

  if (platform->pointer_focus) {
    zui_window_handle_motion(platform->pointer_focus,
                              platform->pointer_x, platform->pointer_y);
  }
}

static void pointer_button(void *data, struct wl_pointer *pointer,
                           uint32_t serial, uint32_t time, uint32_t button,
                           uint32_t state)
{
  ZuiPlatform *platform = data;
  platform->pointer_serial = serial;
  platform->pointer_button = button;

  if (platform->pointer_focus) {
    bool pressed = (state == WL_POINTER_BUTTON_STATE_PRESSED);
    zui_window_handle_button(platform->pointer_focus,
                              platform->pointer_x, platform->pointer_y,
                              button, pressed);
  }
}

static void pointer_axis(void *data, struct wl_pointer *pointer,
                         uint32_t time, uint32_t axis, wl_fixed_t value)
{
  ZuiPlatform *platform = data;
  if (!platform->pointer_focus) return;

  double delta = wl_fixed_to_double(value);
  double dx = 0, dy = 0;

  if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
    dy = delta;
  } else if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
    dx = delta;
  }

  zui_window_handle_scroll(platform->pointer_focus,
                            platform->pointer_x, platform->pointer_y,
                            dx, dy);
}

static void pointer_frame(void *data, struct wl_pointer *pointer) {}
static void pointer_axis_source(void *data, struct wl_pointer *pointer,
                                 uint32_t axis_source) {}
static void pointer_axis_stop(void *data, struct wl_pointer *pointer,
                               uint32_t time, uint32_t axis) {}
static void pointer_axis_discrete(void *data, struct wl_pointer *pointer,
                                   uint32_t axis, int32_t discrete) {}
static void pointer_axis_value120(void *data, struct wl_pointer *pointer,
                                   uint32_t axis, int32_t value120) {}
static void pointer_axis_relative_direction(void *data, struct wl_pointer *pointer,
                                             uint32_t axis, uint32_t direction) {}

static const struct wl_pointer_listener pointer_listener = {
  .enter = pointer_enter,
  .leave = pointer_leave,
  .motion = pointer_motion,
  .button = pointer_button,
  .axis = pointer_axis,
  .frame = pointer_frame,
  .axis_source = pointer_axis_source,
  .axis_stop = pointer_axis_stop,
  .axis_discrete = pointer_axis_discrete,
  .axis_value120 = pointer_axis_value120,
  .axis_relative_direction = pointer_axis_relative_direction,
};

static void keyboard_keymap(void *data, struct wl_keyboard *keyboard,
                            uint32_t format, int fd, uint32_t size)
{
  ZuiPlatform *platform = data;
  (void)keyboard;

  if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
    close(fd);
    return;
  }

  char *map_str = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (map_str == MAP_FAILED) {
    close(fd);
    return;
  }

  if (platform->xkb_keymap) {
    xkb_keymap_unref(platform->xkb_keymap);
  }
  if (platform->xkb_state) {
    xkb_state_unref(platform->xkb_state);
  }

  platform->xkb_keymap = xkb_keymap_new_from_string(
    platform->xkb_context, map_str,
    XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);

  munmap(map_str, size);
  close(fd);

  if (platform->xkb_keymap) {
    platform->xkb_state = xkb_state_new(platform->xkb_keymap);
  }
}

static void keyboard_enter(void *data, struct wl_keyboard *keyboard,
                           uint32_t serial, struct wl_surface *surface,
                           struct wl_array *keys)
{
  ZuiPlatform *platform = data;
  platform->keyboard_serial = serial;
  ZuiWindow *window = wl_surface_get_user_data(surface);
  platform->keyboard_focus = window;
}

static void keyboard_leave(void *data, struct wl_keyboard *keyboard,
                           uint32_t serial, struct wl_surface *surface)
{
  ZuiPlatform *platform = data;
  platform->keyboard_focus = NULL;
}

static void keyboard_key(void *data, struct wl_keyboard *keyboard,
                         uint32_t serial, uint32_t time, uint32_t key,
                         uint32_t state)
{
  ZuiPlatform *platform = data;
  (void)keyboard;
  (void)time;
  platform->keyboard_serial = serial;

  if (!platform->keyboard_focus || !platform->xkb_state) return;

  bool pressed = (state == WL_KEYBOARD_KEY_STATE_PRESSED);
  uint32_t keycode = key + 8;

  xkb_keysym_t sym = xkb_state_key_get_one_sym(platform->xkb_state, keycode);

  char text[8] = {0};
  if (pressed) {
    xkb_state_key_get_utf8(platform->xkb_state, keycode, text, sizeof(text));

    if (platform->repeat_rate > 0 &&
        xkb_keymap_key_repeats(platform->xkb_keymap, keycode)) {
      platform->repeat_key = key;
      platform->repeat_sym = sym;
      memcpy(platform->repeat_text, text, sizeof(text));
      platform->repeat_active = true;
      platform->repeat_next_time = get_time_ms() + (uint64_t)platform->repeat_delay;
    }
  } else {
    if (platform->repeat_key == key) {
      platform->repeat_active = false;
    }
  }

  zui_window_handle_key(platform->keyboard_focus, key, sym, text, pressed);
}

static void keyboard_modifiers(void *data, struct wl_keyboard *keyboard,
                                uint32_t serial, uint32_t mods_depressed,
                                uint32_t mods_latched, uint32_t mods_locked,
                                uint32_t group)
{
  ZuiPlatform *platform = data;
  (void)keyboard;
  (void)serial;

  if (platform->xkb_state) {
    xkb_state_update_mask(platform->xkb_state,
      mods_depressed, mods_latched, mods_locked, 0, 0, group);
  }
}

static void keyboard_repeat_info(void *data, struct wl_keyboard *keyboard,
                                  int32_t rate, int32_t delay)
{
  ZuiPlatform *platform = data;
  (void)keyboard;
  platform->repeat_rate = rate;
  platform->repeat_delay = delay;
}

static const struct wl_keyboard_listener keyboard_listener = {
  .keymap = keyboard_keymap,
  .enter = keyboard_enter,
  .leave = keyboard_leave,
  .key = keyboard_key,
  .modifiers = keyboard_modifiers,
  .repeat_info = keyboard_repeat_info,
};

static void seat_capabilities(void *data, struct wl_seat *seat,
                               uint32_t capabilities)
{
  ZuiPlatform *platform = data;

  bool have_pointer = capabilities & WL_SEAT_CAPABILITY_POINTER;
  bool have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

  if (have_pointer && !platform->pointer) {
    platform->pointer = wl_seat_get_pointer(seat);
    wl_pointer_add_listener(platform->pointer, &pointer_listener, platform);
  } else if (!have_pointer && platform->pointer) {
    wl_pointer_destroy(platform->pointer);
    platform->pointer = NULL;
  }

  if (have_keyboard && !platform->keyboard) {
    platform->keyboard = wl_seat_get_keyboard(seat);
    wl_keyboard_add_listener(platform->keyboard, &keyboard_listener, platform);
  } else if (!have_keyboard && platform->keyboard) {
    wl_keyboard_destroy(platform->keyboard);
    platform->keyboard = NULL;
  }
}

static void seat_name(void *data, struct wl_seat *seat, const char *name) {}

static const struct wl_seat_listener seat_listener = {
  .capabilities = seat_capabilities,
  .name = seat_name,
};

static void registry_global(void *data, struct wl_registry *registry,
                            uint32_t name, const char *interface,
                            uint32_t version)
{
  ZuiPlatform *platform = data;

  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    platform->compositor = wl_registry_bind(registry, name,
                                             &wl_compositor_interface, 4);
  } else if (strcmp(interface, wl_shm_interface.name) == 0) {
    platform->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
  } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    platform->xdg_wm_base = wl_registry_bind(registry, name,
                                              &xdg_wm_base_interface, 1);
    xdg_wm_base_add_listener(platform->xdg_wm_base,
                              &xdg_wm_base_listener, platform);
  } else if (strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0) {
    platform->decoration_manager = wl_registry_bind(registry, name,
                                    &zxdg_decoration_manager_v1_interface, 1);
  } else if (strcmp(interface, wl_seat_interface.name) == 0) {
    platform->seat = wl_registry_bind(registry, name, &wl_seat_interface, 5);
    wl_seat_add_listener(platform->seat, &seat_listener, platform);
  }
}

static void registry_global_remove(void *data, struct wl_registry *registry,
                                    uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
  .global = registry_global,
  .global_remove = registry_global_remove,
};

bool zui_platform_init(ZuiPlatform *platform)
{
  memset(platform, 0, sizeof(*platform));

  platform->display = wl_display_connect(NULL);
  if (!platform->display) {
    fprintf(stderr, "ZUI: Failed to connect to Wayland display\n");
    return false;
  }

  platform->registry = wl_display_get_registry(platform->display);
  wl_registry_add_listener(platform->registry, &registry_listener, platform);

  wl_display_roundtrip(platform->display);

  if (!platform->compositor) {
    fprintf(stderr, "ZUI: No wl_compositor available\n");
    return false;
  }
  if (!platform->xdg_wm_base) {
    fprintf(stderr, "ZUI: No xdg_wm_base available\n");
    return false;
  }

  read_cursor_config();
  const char *cursor_theme = g_cursor_theme[0] ? g_cursor_theme : NULL;

  platform->cursor_theme = wl_cursor_theme_load(cursor_theme, g_cursor_size, platform->shm);
  if (platform->cursor_theme) {
    platform->cursor_surface = wl_compositor_create_surface(platform->compositor);

    platform->cursors[ZUI_CURSOR_DEFAULT] = wl_cursor_theme_get_cursor(platform->cursor_theme, "default");
    platform->cursors[ZUI_CURSOR_POINTER] = wl_cursor_theme_get_cursor(platform->cursor_theme, "pointer");
    platform->cursors[ZUI_CURSOR_TEXT] = wl_cursor_theme_get_cursor(platform->cursor_theme, "text");
    platform->cursors[ZUI_CURSOR_CROSSHAIR] = wl_cursor_theme_get_cursor(platform->cursor_theme, "crosshair");
    platform->cursors[ZUI_CURSOR_MOVE] = wl_cursor_theme_get_cursor(platform->cursor_theme, "move");
    platform->cursors[ZUI_CURSOR_RESIZE_N] = wl_cursor_theme_get_cursor(platform->cursor_theme, "n-resize");
    platform->cursors[ZUI_CURSOR_RESIZE_S] = wl_cursor_theme_get_cursor(platform->cursor_theme, "s-resize");
    platform->cursors[ZUI_CURSOR_RESIZE_E] = wl_cursor_theme_get_cursor(platform->cursor_theme, "e-resize");
    platform->cursors[ZUI_CURSOR_RESIZE_W] = wl_cursor_theme_get_cursor(platform->cursor_theme, "w-resize");
    platform->cursors[ZUI_CURSOR_RESIZE_NE] = wl_cursor_theme_get_cursor(platform->cursor_theme, "ne-resize");
    platform->cursors[ZUI_CURSOR_RESIZE_NW] = wl_cursor_theme_get_cursor(platform->cursor_theme, "nw-resize");
    platform->cursors[ZUI_CURSOR_RESIZE_SE] = wl_cursor_theme_get_cursor(platform->cursor_theme, "se-resize");
    platform->cursors[ZUI_CURSOR_RESIZE_SW] = wl_cursor_theme_get_cursor(platform->cursor_theme, "sw-resize");
    platform->cursors[ZUI_CURSOR_NOT_ALLOWED] = wl_cursor_theme_get_cursor(platform->cursor_theme, "not-allowed");
    platform->cursors[ZUI_CURSOR_GRAB] = wl_cursor_theme_get_cursor(platform->cursor_theme, "grab");
    platform->cursors[ZUI_CURSOR_GRABBING] = wl_cursor_theme_get_cursor(platform->cursor_theme, "grabbing");
    platform->cursors[ZUI_CURSOR_COL_RESIZE] = wl_cursor_theme_get_cursor(platform->cursor_theme, "col-resize");
    platform->cursors[ZUI_CURSOR_ROW_RESIZE] = wl_cursor_theme_get_cursor(platform->cursor_theme, "row-resize");

    if (!platform->cursors[ZUI_CURSOR_DEFAULT]) {
      platform->cursors[ZUI_CURSOR_DEFAULT] = wl_cursor_theme_get_cursor(platform->cursor_theme, "left_ptr");
    }
    if (!platform->cursors[ZUI_CURSOR_POINTER]) {
      platform->cursors[ZUI_CURSOR_POINTER] = wl_cursor_theme_get_cursor(platform->cursor_theme, "hand1");
    }

    platform->current_cursor = ZUI_CURSOR_DEFAULT;
  }

  platform->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (!platform->xkb_context) {
    fprintf(stderr, "ZUI: Failed to create xkb context\n");
  }

  platform->running = true;
  return true;
}

void zui_platform_shutdown(ZuiPlatform *platform)
{
  if (platform->xkb_state)
    xkb_state_unref(platform->xkb_state);
  if (platform->xkb_keymap)
    xkb_keymap_unref(platform->xkb_keymap);
  if (platform->xkb_context)
    xkb_context_unref(platform->xkb_context);
  if (platform->cursor_surface)
    wl_surface_destroy(platform->cursor_surface);
  if (platform->cursor_theme)
    wl_cursor_theme_destroy(platform->cursor_theme);
  if (platform->keyboard)
    wl_keyboard_destroy(platform->keyboard);
  if (platform->pointer)
    wl_pointer_destroy(platform->pointer);
  if (platform->seat)
    wl_seat_destroy(platform->seat);
  if (platform->decoration_manager)
    zxdg_decoration_manager_v1_destroy(platform->decoration_manager);
  if (platform->xdg_wm_base)
    xdg_wm_base_destroy(platform->xdg_wm_base);
  if (platform->shm)
    wl_shm_destroy(platform->shm);
  if (platform->compositor)
    wl_compositor_destroy(platform->compositor);
  if (platform->registry)
    wl_registry_destroy(platform->registry);
  if (platform->display)
    wl_display_disconnect(platform->display);
}

void zui_platform_poll_events(ZuiPlatform *platform)
{
  wl_display_dispatch_pending(platform->display);
  wl_display_flush(platform->display);

  struct pollfd pfd = {
    .fd = wl_display_get_fd(platform->display),
    .events = POLLIN,
  };

  int timeout = 0;
  if (platform->repeat_active && platform->keyboard_focus) {
    uint64_t now = get_time_ms();
    if (now >= platform->repeat_next_time) {
      zui_window_handle_key(platform->keyboard_focus,
        platform->repeat_key, platform->repeat_sym,
        platform->repeat_text, true);

      if (platform->repeat_rate > 0) {
        platform->repeat_next_time = now + (1000 / (uint64_t)platform->repeat_rate);
      }
    }
  }

  if (poll(&pfd, 1, timeout) > 0) {
    wl_display_dispatch(platform->display);
  }
}

static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface,
                                   uint32_t serial)
{
  ZuiWindow *window = data;
  ZuiWaylandWindow *wl_win = zui_window_get_wayland(window);

  xdg_surface_ack_configure(xdg_surface, serial);

  if (wl_win->pending_resize) {
    wl_egl_window_resize(wl_win->egl_window,
                         wl_win->pending_width, wl_win->pending_height, 0, 0);
    zui_window_handle_resize(window, wl_win->pending_width,
                              wl_win->pending_height);
    wl_win->pending_resize = false;
  }

  wl_win->configured = true;
}

static const struct xdg_surface_listener xdg_surface_listener = {
  .configure = xdg_surface_configure,
};

static void xdg_toplevel_configure(void *data, struct xdg_toplevel *toplevel,
                                    int32_t width, int32_t height,
                                    struct wl_array *states)
{
  ZuiWindow *window = data;
  ZuiWaylandWindow *wl_win = zui_window_get_wayland(window);

  bool maximized = false;
  bool fullscreen = false;
  bool activated = false;
  uint32_t *state;
  wl_array_for_each(state, states) {
    if (*state == XDG_TOPLEVEL_STATE_MAXIMIZED)
      maximized = true;
    if (*state == XDG_TOPLEVEL_STATE_FULLSCREEN)
      fullscreen = true;
    if (*state == XDG_TOPLEVEL_STATE_ACTIVATED)
      activated = true;
  }
  zui_window_set_maximized_state(window, maximized);
  zui_window_set_fullscreen_state(window, fullscreen);
  zui_window_set_active_state(window, activated);

  if (width > 0 && height > 0) {
    wl_win->pending_width = width;
    wl_win->pending_height = height;
    wl_win->pending_resize = true;
  }
}

static void xdg_toplevel_close(void *data, struct xdg_toplevel *toplevel)
{
  ZuiWindow *window = data;
  zui_window_request_close(window);
}

static void xdg_toplevel_configure_bounds(void *data,
                                           struct xdg_toplevel *toplevel,
                                           int32_t width, int32_t height)
{
}

static void xdg_toplevel_wm_capabilities(void *data,
                                          struct xdg_toplevel *toplevel,
                                          struct wl_array *capabilities)
{
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
  .configure = xdg_toplevel_configure,
  .close = xdg_toplevel_close,
  .configure_bounds = xdg_toplevel_configure_bounds,
  .wm_capabilities = xdg_toplevel_wm_capabilities,
};

static void decoration_configure(void *data,
                                  struct zxdg_toplevel_decoration_v1 *decoration,
                                  uint32_t mode)
{
  (void)decoration;
  ZuiWindow *window = data;
  if (!window) return;

  bool client_side = (mode == ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
  zui_window_set_decorated_state(window, client_side);
}

static const struct zxdg_toplevel_decoration_v1_listener decoration_listener = {
  .configure = decoration_configure,
};

bool zui_wayland_window_create(ZuiPlatform *platform, ZuiWaylandWindow *wl_win,
                                ZuiWindow *window, int width, int height,
                                const char *title)
{
  memset(wl_win, 0, sizeof(*wl_win));

  wl_win->surface = wl_compositor_create_surface(platform->compositor);
  if (!wl_win->surface) {
    fprintf(stderr, "ZUI: Failed to create Wayland surface\n");
    return false;
  }
  wl_surface_set_user_data(wl_win->surface, window);

  wl_win->xdg_surface = xdg_wm_base_get_xdg_surface(platform->xdg_wm_base,
                                                     wl_win->surface);
  xdg_surface_add_listener(wl_win->xdg_surface, &xdg_surface_listener, window);

  wl_win->xdg_toplevel = xdg_surface_get_toplevel(wl_win->xdg_surface);
  xdg_toplevel_add_listener(wl_win->xdg_toplevel, &xdg_toplevel_listener, window);
  xdg_toplevel_set_title(wl_win->xdg_toplevel, title);
  xdg_toplevel_set_app_id(wl_win->xdg_toplevel, "zui.app");

  if (platform->decoration_manager) {
    wl_win->decoration = zxdg_decoration_manager_v1_get_toplevel_decoration(
      platform->decoration_manager, wl_win->xdg_toplevel);
    zxdg_toplevel_decoration_v1_add_listener(wl_win->decoration,
                                              &decoration_listener, window);
    zxdg_toplevel_decoration_v1_set_mode(wl_win->decoration,
      ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
  }

  wl_win->egl_window = wl_egl_window_create(wl_win->surface, width, height);
  if (!wl_win->egl_window) {
    fprintf(stderr, "ZUI: Failed to create EGL window\n");
    return false;
  }

  wl_win->pending_width = width;
  wl_win->pending_height = height;

  wl_surface_commit(wl_win->surface);
  wl_display_roundtrip(platform->display);

  return true;
}

void zui_wayland_window_destroy(ZuiPlatform *platform, ZuiWaylandWindow *wl_win)
{
  if (wl_win->frame_callback)
    wl_callback_destroy(wl_win->frame_callback);
  if (wl_win->egl_window)
    wl_egl_window_destroy(wl_win->egl_window);
  if (wl_win->decoration)
    zxdg_toplevel_decoration_v1_destroy(wl_win->decoration);
  if (wl_win->xdg_toplevel)
    xdg_toplevel_destroy(wl_win->xdg_toplevel);
  if (wl_win->xdg_surface)
    xdg_surface_destroy(wl_win->xdg_surface);
  if (wl_win->surface)
    wl_surface_destroy(wl_win->surface);
}

void zui_wayland_window_set_title(ZuiWaylandWindow *wl_win, const char *title)
{
  xdg_toplevel_set_title(wl_win->xdg_toplevel, title);
}

static void frame_callback_done(void *data, struct wl_callback *callback,
                                 uint32_t time);

static const struct wl_callback_listener frame_listener = {
  .done = frame_callback_done,
};

static void frame_callback_done(void *data, struct wl_callback *callback,
                                 uint32_t time)
{
  ZuiWindow *window = data;
  ZuiWaylandWindow *wl_win = zui_window_get_wayland(window);

  wl_callback_destroy(callback);
  wl_win->frame_callback = NULL;

  zui_window_mark_needs_redraw(window);
}

void zui_wayland_window_request_frame(ZuiWaylandWindow *wl_win, ZuiWindow *window)
{
  if (wl_win->frame_callback)
    return;

  wl_win->frame_callback = wl_surface_frame(wl_win->surface);
  wl_callback_add_listener(wl_win->frame_callback, &frame_listener, window);
  wl_surface_commit(wl_win->surface);
}

void zui_wayland_start_move(ZuiPlatform *platform, ZuiWaylandWindow *wl_win)
{
  xdg_toplevel_move(wl_win->xdg_toplevel, platform->seat,
                     platform->pointer_serial);
}

void zui_wayland_start_resize(ZuiPlatform *platform, ZuiWaylandWindow *wl_win,
                               uint32_t edges)
{
  xdg_toplevel_resize(wl_win->xdg_toplevel, platform->seat,
                       platform->pointer_serial, edges);
}

void zui_wayland_minimize(ZuiWaylandWindow *wl_win)
{
  xdg_toplevel_set_minimized(wl_win->xdg_toplevel);
}

void zui_wayland_maximize(ZuiWaylandWindow *wl_win)
{
  xdg_toplevel_set_maximized(wl_win->xdg_toplevel);
}

void zui_wayland_unmaximize(ZuiWaylandWindow *wl_win)
{
  xdg_toplevel_unset_maximized(wl_win->xdg_toplevel);
}

void zui_wayland_set_min_size(ZuiWaylandWindow *wl_win, int width, int height)
{
  if (wl_win->xdg_toplevel) {
    xdg_toplevel_set_min_size(wl_win->xdg_toplevel, width, height);
  }
}

void zui_wayland_set_max_size(ZuiWaylandWindow *wl_win, int width, int height)
{
  if (wl_win->xdg_toplevel) {
    xdg_toplevel_set_max_size(wl_win->xdg_toplevel, width, height);
  }
}

void zui_wayland_show_window_menu(ZuiPlatform *platform, ZuiWaylandWindow *wl_win,
                                   int x, int y)
{
  if (wl_win->xdg_toplevel && platform->seat) {
    xdg_toplevel_show_window_menu(wl_win->xdg_toplevel, platform->seat,
                                   platform->pointer_serial, x, y);
  }
}

void zui_wayland_set_fullscreen(ZuiWaylandWindow *wl_win, bool fullscreen)
{
  if (!wl_win->xdg_toplevel) return;

  if (fullscreen) {
    xdg_toplevel_set_fullscreen(wl_win->xdg_toplevel, NULL);
  } else {
    xdg_toplevel_unset_fullscreen(wl_win->xdg_toplevel);
  }
}

void zui_wayland_set_decorated(ZuiPlatform *platform, ZuiWaylandWindow *wl_win,
                                bool decorated)
{
  (void)platform;
  if (!wl_win->decoration) return;

  if (decorated) {
    zxdg_toplevel_decoration_v1_set_mode(wl_win->decoration,
      ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
  } else {
    zxdg_toplevel_decoration_v1_set_mode(wl_win->decoration,
      ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
  }
}

void zui_platform_set_cursor(ZuiPlatform *platform, ZuiCursor cursor)
{
  if (!platform->cursor_theme || !platform->pointer) return;
  if (cursor == platform->current_cursor) return;
  if (cursor < 0 || cursor >= ZUI_CURSOR_COUNT) return;

  struct wl_cursor *wl_cursor = platform->cursors[cursor];
  if (!wl_cursor) {
    wl_cursor = platform->cursors[ZUI_CURSOR_DEFAULT];
  }
  if (!wl_cursor) return;

  struct wl_cursor_image *image = wl_cursor->images[0];
  struct wl_buffer *buffer = wl_cursor_image_get_buffer(image);

  wl_pointer_set_cursor(platform->pointer, platform->pointer_enter_serial,
                         platform->cursor_surface,
                         (int32_t)image->hotspot_x, (int32_t)image->hotspot_y);

  wl_surface_attach(platform->cursor_surface, buffer, 0, 0);
  wl_surface_damage_buffer(platform->cursor_surface, 0, 0,
                            (int32_t)image->width, (int32_t)image->height);
  wl_surface_commit(platform->cursor_surface);

  platform->current_cursor = cursor;
}

bool zui_platform_get_ctrl(ZuiPlatform *platform)
{
  if (!platform || !platform->xkb_state) return false;
  return xkb_state_mod_name_is_active(platform->xkb_state,
    XKB_MOD_NAME_CTRL, XKB_STATE_MODS_EFFECTIVE) > 0;
}

bool zui_platform_get_shift(ZuiPlatform *platform)
{
  if (!platform || !platform->xkb_state) return false;
  return xkb_state_mod_name_is_active(platform->xkb_state,
    XKB_MOD_NAME_SHIFT, XKB_STATE_MODS_EFFECTIVE) > 0;
}

bool zui_platform_get_alt(ZuiPlatform *platform)
{
  if (!platform || !platform->xkb_state) return false;
  return xkb_state_mod_name_is_active(platform->xkb_state,
    XKB_MOD_NAME_ALT, XKB_STATE_MODS_EFFECTIVE) > 0;
}
