# Quick Start

Get up and running with ZUI in under 5 minutes.

## Minimal Example

Here's the simplest possible ZUI application:

```c
#include <zui/zui.h>

int main(void)
{
    // Initialize ZUI
    zui_init();
    
    // Create a window
    ZuiWindow *window = zui_window_create(800, 600, "My App");
    zui_window_show(window);
    
    // Main loop
    while (zui_window_running(window)) {
        zui_window_render(window);
        zui_poll_events();
    }
    
    // Cleanup
    zui_window_destroy(window);
    zui_shutdown();
    
    return 0;
}
```

## Adding Widgets

Let's add a button that responds to clicks:

```c
#include <zui/zui.h>
#include <stdio.h>

void on_button_click(ZuiWidget *widget, void *data)
{
    printf("Button clicked!\n");
}

int main(void)
{
    zui_init();
    
    ZuiWindow *window = zui_window_create(800, 600, "Button Example");
    
    // Create a button
    ZuiWidget *button = zui_button_new("Click Me!");
    zui_button_on_click(ZUI_BUTTON(button), on_button_click, NULL);
    
    // Add button to window content
    ZuiWidget *content = zui_window_content(window);
    zui_widget_add_child(content, button);
    
    zui_window_show(window);
    
    while (zui_window_running(window)) {
        zui_window_render(window);
        zui_poll_events();
    }
    
    zui_window_destroy(window);
    zui_shutdown();
    
    return 0;
}
```

## Using Panels for Layout

Panels help organize widgets:

```c
#include <zui/zui.h>

int main(void)
{
    zui_init();
    
    ZuiWindow *window = zui_window_create(800, 600, "Layout Example");
    
    // Create a vertical panel
    ZuiWidget *panel = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(panel), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(panel), 12);
    zui_panel_set_padding(ZUI_PANEL(panel), 20, 20, 20, 20);
    zui_panel_set_fill(ZUI_PANEL(panel), true, true);
    
    // Create widgets
    ZuiWidget *title = zui_label_new("Welcome!");
    ZuiWidget *subtitle = zui_label_new("This is ZUI");
    ZuiWidget *button = zui_button_new("Get Started");
    
    // Style the title
    zui_label_set_size(ZUI_LABEL(title), 24);
    zui_label_set_color(ZUI_LABEL(title), ZUI_COLOR_HEX(0xffffff));
    
    // Add widgets to panel
    zui_panel_add_child(ZUI_PANEL(panel), title);
    zui_panel_add_child(ZUI_PANEL(panel), subtitle);
    zui_panel_add_child(ZUI_PANEL(panel), button);
    
    // Add panel to window
    zui_widget_add_child(zui_window_content(window), panel);
    
    // Set window background
    zui_window_set_background_color(window, ZUI_COLOR_HEX(0x1a1a2e));
    
    zui_window_show(window);
    
    while (zui_window_running(window)) {
        zui_window_render(window);
        zui_poll_events();
    }
    
    zui_window_destroy(window);
    zui_shutdown();
    
    return 0;
}
```

## The Widget Type System

ZUI uses a GTK-like type system. All widgets share a common base type `ZuiWidget *`:

```c
// Create widgets - returns ZuiWidget *
ZuiWidget *button = zui_button_new("Click");
ZuiWidget *label = zui_label_new("Hello");
ZuiWidget *panel = zui_panel_new();

// Type checking
if (ZUI_IS_BUTTON(widget)) {
    printf("It's a button!\n");
}

// Safe downcasting (returns NULL if type doesn't match)
ZuiButton *btn = ZUI_BUTTON(widget);
if (btn) {
    zui_button_set_text(btn, "New Text");
}

// Generic operations work on any widget
zui_widget_set_visible(button, false);
zui_widget_set_background(panel, ZUI_COLOR_HEX(0x333333));
```

## Colors

ZUI provides several ways to define colors:

```c
// RGBA (0.0 - 1.0)
ZuiColor red = ZUI_COLOR(1.0f, 0.0f, 0.0f, 1.0f);

// Hex color
ZuiColor blue = ZUI_COLOR_HEX(0x3498db);

// RGB shorthand
ZuiColor white = ZUI_COLOR_RGB(1.0f, 1.0f, 1.0f);

// With alpha
ZuiColor transparent_black = ZUI_COLOR(0, 0, 0, 0.5f);
```

## Next Steps

- **[Your First App](first-app.md)** — Build a complete application
- **[Widget Overview](../widgets/overview.md)** — Explore available widgets
- **[Layout System](../guides/layout.md)** — Master layouts
- **[Event Handling](../guides/events.md)** — Handle user input

---

**Next:** [Your First App →](first-app.md)
