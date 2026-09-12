# ZUI Documentation

**ZUI** is a lightweight, modern GUI toolkit designed for building beautiful, performant applications on Linux with Wayland.

<div class="hero-features">

- **Lightweight** — Minimal dependencies, small footprint
- **Modern** — Clean C API with GTK-like widget model
- **Fast** — Hardware-accelerated OpenGL rendering
- **Native** — First-class Wayland support
- **Extensible** — Easy to customize and extend

</div>

## Quick Example

```c
#include <zui/zui.h>

int main(void)
{
    zui_init();
    
    ZuiWindow *window = zui_window_create(800, 600, "Hello ZUI");
    
    // Create widgets using the unified API
    ZuiWidget *panel = zui_panel_new();
    ZuiWidget *button = zui_button_new("Click Me!");
    ZuiWidget *label = zui_label_new("Welcome to ZUI");
    
    // Configure the panel
    zui_panel_set_layout(ZUI_PANEL(panel), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(panel), 16);
    zui_panel_set_alignment(ZUI_PANEL(panel), ZUI_ALIGN_CENTER, ZUI_ALIGN_CENTER);
    
    // Add widgets to panel
    zui_panel_add_child(ZUI_PANEL(panel), label);
    zui_panel_add_child(ZUI_PANEL(panel), button);
    
    // Add panel to window
    zui_widget_add_child(zui_window_content(window), panel);
    
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

## Documentation

<div class="doc-grid">

### Getting Started
- [Installation](getting-started/installation.md)
- [Quick Start](getting-started/quick-start.md)
- [Your First App](getting-started/first-app.md)

### Core Concepts
- [Architecture](architecture.md)
- [Widget System](widgets/overview.md)
- [Event Handling](guides/events.md)
- [Layout System](guides/layout.md)

### Widgets
- [Overview](widgets/overview.md)
- [Button](widgets/button.md)
- [Label](widgets/label.md)
- [Panel](widgets/panel.md)
- [TextInput](widgets/textinput.md)
- [All Widgets →](widgets/overview.md#widget-catalog)

### Guides
- [Styling & Theming](guides/styling.md)
- [Custom Widgets](guides/custom-widgets.md)
- [Window Management](guides/windows.md)
- [Audio & Media](guides/media.md)

### API Reference
- [Full API Reference](api-reference.md)
- [Type Macros](api-reference.md#type-macros)
- [Widget Functions](api-reference.md#widget-functions)

</div>

## Design Philosophy

ZUI follows these core principles:

> **Simple things should be simple.**

Creating a window with a button should take a few lines of code, not dozens.

> **Lightweight by default.**

No bloated dependencies. Just what you need for modern GUI development.

> **Performance without complexity.**

Hardware-accelerated rendering with an intuitive API.

## System Requirements

- **OS**: Linux (Wayland compositor required)
- **Graphics**: OpenGL 3.3+ capable GPU
- **Compiler**: GCC or Clang with C11 support
- **Build**: CMake 3.16+

## License

ZUI is open source software licensed under the MIT License.

---

<div class="footer-nav">

**Next:** [Installation →](getting-started/installation.md)

</div>
