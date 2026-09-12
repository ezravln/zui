# Window Management

This guide covers creating and managing windows in ZUI applications.

## Creating a Window

```c
ZuiWindow *window = zui_window_create(800, 600, "My Application");

if (!window) {
    fprintf(stderr, "Failed to create window\n");
    return 1;
}
```

By default, a window is created **without decoration** (no titlebar, no buttons). This gives you full control over the window appearance.

## Window Decoration

ZUI provides a flexible window decoration system. You can either:
1. Use the default window decoration
2. Build your own custom decoration
3. Have no decoration at all

### Default Window Decoration

The easiest way to add standard window controls:

```c
ZuiWindowDecoration *decor = zui_default_window_decoration(window);

// Set the window title (displayed in center)
zui_window_decoration_set_title(decor, "My Application");

// Optionally set a logo (displayed on the left)
zui_window_decoration_set_logo(decor, "assets/logo.png");
```

The default decoration includes:
- Drag-to-move functionality (click anywhere on decoration to move window)
- Hide button (hide to system tray)
- Minimize button
- Maximize/Restore button
- Close button

### Custom Window Decoration

For full control, build your own decoration using handler registration:

```c
// Create your own titlebar widget
ZuiWidget *my_titlebar = zui_panel_new();
zui_panel_set_layout(ZUI_PANEL(my_titlebar), ZUI_LAYOUT_HORIZONTAL);
zui_panel_set_fill(ZUI_PANEL(my_titlebar), true, false);
zui_widget_set_size(ZUI_WIDGET(my_titlebar), 0, 40);
zui_widget_set_background(ZUI_WIDGET(my_titlebar), ZUI_COLOR_HEX(0x2d2d2d));

// Register it as a moving handler (drag to move window)
zui_add_window_moving_handler(window, my_titlebar);

// Create custom close button
ZuiWidget *close_btn = zui_button_new("X");
zui_add_close_window_handler(window, close_btn);

// Add to content
ZuiWidget *content = zui_window_content(window);
zui_widget_add_child(content, my_titlebar);
```

### Handler Registration API

Register any widget to trigger window actions:

```c
// Moving - drag widget to move window
zui_add_window_moving_handler(window, widget);
zui_remove_window_moving_handler(window, widget);

// Close - click widget to close window
zui_add_close_window_handler(window, widget);
zui_remove_close_window_handler(window, widget);

// Maximize/Restore - click widget to toggle maximize
zui_add_window_maximize_handler(window, widget);
zui_remove_window_maximize_handler(window, widget);

// Hide - click widget to hide to system tray
zui_add_hide_window_handler(window, widget);
zui_remove_hide_window_handler(window, widget);

// Minimize - click widget to minimize to taskbar
zui_add_minimize_window_handler(window, widget);
zui_remove_minimize_window_handler(window, widget);
```

Multiple widgets can have the same handler:

```c
// Both logo and titlebar area can move the window
zui_add_window_moving_handler(window, logo_widget);
zui_add_window_moving_handler(window, titlebar_panel);
```

### Pre-built Control Buttons

ZUI provides styled buttons for window controls:

```c
ZuiWidget *close_btn = zui_window_close_button(window);
ZuiWidget *minimize_btn = zui_window_minimize_button(window);
ZuiWidget *maximize_btn = zui_window_maximize_button(window);
ZuiWidget *hide_btn = zui_window_hide_button(window);
```

## Window Lifecycle

### Basic Lifecycle

```c
// 1. Initialize ZUI
zui_init();

// 2. Create window
ZuiWindow *window = zui_window_create(800, 600, "App");

// 3. Add decoration (optional)
ZuiWindowDecoration *decor = zui_default_window_decoration(window);
zui_window_decoration_set_title(decor, "App");

// 4. Configure window
zui_window_set_background_color(window, ZUI_COLOR_HEX(0x1a1a2e));

// 5. Build UI
ZuiWidget *content = zui_window_content(window);
// ... add widgets ...

// 6. Show window
zui_window_show(window);

// 7. Main loop
while (zui_window_running(window)) {
    zui_window_render(window);
    zui_poll_events();
}

// 8. Cleanup
zui_window_decoration_destroy(decor);
zui_window_destroy(window);
zui_shutdown();
```

## Window Properties

### Title

```c
zui_window_set_title(window, "New Title");
```

### Size Constraints

```c
// Minimum size
zui_window_set_min_size(window, 400, 300);

// Maximum size
zui_window_set_max_size(window, 1200, 900);
```

### Get Current Size

```c
int width, height;
zui_window_get_size(window, &width, &height);
printf("Window size: %dx%d\n", width, height);
```

### Background Color

```c
zui_window_set_background_color(window, ZUI_COLOR_HEX(0x1a1a2e));
```

### Corner Radius

For modern rounded window appearance:

```c
zui_window_set_corner_radius(window, 16.0f);
```

## Window State

### Query State

```c
bool maximized = zui_window_is_maximized(window);
bool minimized = zui_window_is_minimized(window);
bool hidden = zui_window_is_hidden(window);
bool fullscreen = zui_window_is_fullscreen(window);
bool active = zui_window_is_active(window);
```

### Programmatic Control

```c
zui_window_maximize(window);   // Toggle maximize/restore
zui_window_minimize(window);   // Minimize to taskbar
zui_window_hide(window);       // Hide to system tray
zui_window_close(window);      // Close window

zui_window_set_fullscreen(window, true);   // Enter fullscreen
zui_window_set_fullscreen(window, false);  // Exit fullscreen
```

## Window Structure

A ZUI window consists of a content area where you add your UI:

```
┌─────────────────────────────────┐
│                                 │
│           Content               │
│    (your UI goes here)          │
│                                 │
└─────────────────────────────────┘
```

With default decoration:

```
┌─────────────────────────────────┐
│ [Logo] Title      [_][□][X]     │  ← Window Decoration
├─────────────────────────────────┤
│                                 │
│           Content               │
│    (your UI goes here)          │
│                                 │
└─────────────────────────────────┘
```

### Content Area

The content area is where your UI goes:

```c
ZuiWidget *content = zui_window_content(window);

// Add your UI
ZuiWidget *main_panel = zui_panel_new();
zui_widget_add_child(content, main_panel);
```

## Responsive Design

### Handling Resize

```c
int last_width = 0, last_height = 0;

while (zui_window_running(window)) {
    int w, h;
    zui_window_get_size(window, &w, &h);
    
    if (w != last_width || h != last_height) {
        // Window was resized
        update_layout_for_size(w, h);
        last_width = w;
        last_height = h;
    }
    
    zui_window_render(window);
    zui_poll_events();
}
```

## Example: Complete Window Setup

```c
#include <zui/zui.h>
#include <stdio.h>

typedef struct {
    ZuiWindow *window;
    ZuiWindowDecoration *decor;
    ZuiWidget *main_panel;
} App;

static App app;

ZuiWidget *create_main_ui(void)
{
    app.main_panel = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(app.main_panel), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(app.main_panel), 16);
    zui_panel_set_padding(ZUI_PANEL(app.main_panel), 20, 20, 20, 20);
    zui_panel_set_fill(ZUI_PANEL(app.main_panel), true, true);
    zui_panel_set_alignment(ZUI_PANEL(app.main_panel), 
        ZUI_ALIGN_CENTER, ZUI_ALIGN_CENTER);
    
    ZuiWidget *welcome = zui_label_new("Welcome to ZUI!");
    zui_label_set_size(ZUI_LABEL(welcome), 28);
    zui_label_set_color(ZUI_LABEL(welcome), ZUI_COLOR_HEX(0xffffff));
    
    ZuiWidget *button = zui_button_new("Click Me");
    
    zui_panel_add_child(ZUI_PANEL(app.main_panel), welcome);
    zui_panel_add_child(ZUI_PANEL(app.main_panel), button);
    
    return app.main_panel;
}

int main(void)
{
    if (!zui_init()) {
        fprintf(stderr, "Failed to initialize ZUI\n");
        return 1;
    }
    
    app.window = zui_window_create(800, 600, "Window Example");
    if (!app.window) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }
    
    // Configure window
    zui_window_set_min_size(app.window, 400, 300);
    zui_window_set_background_color(app.window, ZUI_COLOR_HEX(0x1a1a2e));
    zui_window_set_corner_radius(app.window, 12);
    
    // Add default window decoration
    app.decor = zui_default_window_decoration(app.window);
    zui_window_decoration_set_title(app.decor, "Window Example");
    
    // Setup content
    ZuiWidget *content = zui_window_content(app.window);
    zui_widget_add_child(content, create_main_ui());
    
    zui_window_show(app.window);
    
    while (zui_window_running(app.window)) {
        zui_window_render(app.window);
        zui_poll_events();
    }
    
    zui_window_decoration_destroy(app.decor);
    zui_window_destroy(app.window);
    zui_shutdown();
    
    return 0;
}
```

## Best Practices

1. **Choose the right decoration approach**
   - Default decoration for standard apps
   - Custom decoration for unique branding
   - No decoration for fullscreen/kiosk apps

2. **Always check window creation** — It can fail if Wayland isn't available

3. **Set sensible size constraints** — Prevent unusable window sizes

4. **Handle resize gracefully** — Update layouts when size changes

5. **Clean up properly** — Destroy decoration before destroying window

---

**See Also:** [Quick Start](../getting-started/quick-start.md) · [Layout Guide](layout.md) · [Event Handling](events.md)
