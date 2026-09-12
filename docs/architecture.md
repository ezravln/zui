# Architecture

This document describes ZUI's internal architecture and design decisions.

## Overview

ZUI is structured as a layered architecture that separates concerns and enables future platform portability.

```
┌─────────────────────────────────────────────────┐
│                  Application                     │
├─────────────────────────────────────────────────┤
│                 ZUI Public API                   │
├─────────────────────────────────────────────────┤
│    Widget System    │    Event System           │
├─────────────────────┼───────────────────────────┤
│      Renderer       │    Platform Layer         │
├─────────────────────┴───────────────────────────┤
│     OpenGL/EGL      │       Wayland             │
└─────────────────────┴───────────────────────────┘
```

## Core Components

### Public API Layer

The public API provides a clean, stable interface for application developers:

```c
// Simple, intuitive functions
ZuiWindow *window = zui_window_create(800, 600, "App");
ZuiWidget *button = zui_button_new("Click");
```

Design principles:
- **Minimal boilerplate** — Common tasks require few lines of code
- **Consistent naming** — `zui_<widget>_<action>()` pattern
- **Type safety** — GTK-like type macros for safe casting
- **No global state leakage** — Explicit initialization and cleanup

### Widget System

The widget system follows an object-oriented design using C:

```
ZuiWidget (base)
    ├── ZuiButton
    ├── ZuiLabel
    ├── ZuiPanel
    ├── ZuiTextInput
    ├── ZuiSlider
    ├── ZuiCheckbox
    └── ... (30+ widget types)
```

#### Widget Structure

Every widget embeds `ZuiWidget` as its first member:

```c
struct ZuiButton {
    ZuiWidget base;      // Must be first
    char *text;
    ZuiColor normal_color;
    ZuiColor hover_color;
    // ...
};
```

This enables safe casting between `ZuiWidget *` and specific widget types.

#### Widget Type System

Each widget has a type identifier:

```c
typedef enum {
    ZUI_WIDGET_WINDOW,
    ZUI_WIDGET_BUTTON,
    ZUI_WIDGET_LABEL,
    ZUI_WIDGET_PANEL,
    // ...
} ZuiWidgetType;
```

Type checking macros:

```c
#define ZUI_IS_BUTTON(w)  (zui_widget_get_type(w) == ZUI_WIDGET_BUTTON)
#define ZUI_BUTTON(w)     (ZUI_IS_BUTTON(w) ? (ZuiButton *)(w) : NULL)
```

#### Virtual Table (VTable)

Widgets use a vtable for polymorphic behavior:

```c
typedef struct ZuiWidgetVTable {
    void (*draw)(ZuiWidget *widget, ZuiRenderer *renderer);
    void (*layout)(ZuiWidget *widget);
    void (*destroy)(ZuiWidget *widget);
    bool (*hit_test)(ZuiWidget *widget, float x, float y);
    void (*on_mouse_enter)(ZuiWidget *widget);
    void (*on_mouse_leave)(ZuiWidget *widget);
    void (*on_mouse_down)(ZuiWidget *widget, float x, float y, uint32_t button);
    void (*on_mouse_up)(ZuiWidget *widget, float x, float y, uint32_t button);
    void (*on_key)(ZuiWidget *widget, uint32_t key, uint32_t sym, 
                   const char *text, bool pressed);
    // ...
} ZuiWidgetVTable;
```

### Widget Tree

Widgets form a parent-child hierarchy:

```
Window
└── Content (ZuiWidget)
    └── Panel
        ├── Label
        ├── Button
        └── Panel
            ├── TextInput
            └── Button
```

Operations cascade through the tree:
- **Layout**: Parent lays out children, then children lay out their children
- **Drawing**: Parent draws first, then children (painter's algorithm)
- **Events**: Events bubble from deepest hit widget up to root

### Renderer

The renderer is responsible for all drawing operations:

```c
// Primitive operations
zui_renderer_draw_rect(renderer, rect, color);
zui_renderer_draw_rounded_rect(renderer, rect, color, radius);
zui_renderer_draw_text(renderer, text, x, y, font, color);
zui_renderer_draw_texture(renderer, texture, rect, tint);

// State management
zui_renderer_push_clip(renderer, rect, radius);
zui_renderer_pop_clip(renderer);
```

#### Rendering Pipeline

1. **Clear**: Clear the framebuffer
2. **Layout**: Recalculate widget positions if needed
3. **Draw**: Traverse widget tree, drawing each widget
4. **Swap**: Present the framebuffer

```c
void zui_window_render(ZuiWindow *window)
{
    if (window->needs_layout) {
        zui_widget_layout((ZuiWidget *)window);
    }
    
    zui_renderer_begin(window->renderer);
    zui_widget_draw((ZuiWidget *)window, window->renderer);
    zui_renderer_end(window->renderer);
    
    eglSwapBuffers(window->egl_display, window->egl_surface);
}
```

### Platform Layer

The platform layer abstracts OS-specific functionality:

```
platform/
└── wayland/
    ├── display.c      # Wayland connection
    ├── window.c       # Window management
    ├── input.c        # Keyboard/pointer input
    ├── cursor.c       # Cursor management
    └── egl.c          # EGL context setup
```

#### Wayland Architecture

```
┌─────────────────────────────────────┐
│            Compositor               │
│  ┌─────────┐  ┌─────────┐          │
│  │ Output  │  │ Output  │          │
│  └─────────┘  └─────────┘          │
└──────────────────────────────────────┘
        ↑ Wayland Protocol ↑
┌─────────────────────────────────────┐
│          ZUI Application            │
│  ┌─────────┐  ┌─────────┐          │
│  │ Window  │  │ Window  │          │
│  └─────────┘  └─────────┘          │
└─────────────────────────────────────┘
```

Key Wayland interfaces used:
- `wl_display` — Connection to compositor
- `wl_compositor` — Surface creation
- `xdg_wm_base` — Window management (XDG Shell)
- `wl_seat` — Input devices
- `wl_keyboard` / `wl_pointer` — Input events

### Event System

Events flow from the platform layer through widgets:

```
Wayland Event → Platform Layer → ZUI Event → Widget Handler
```

#### Event Types

```c
// Internal event structure (simplified)
typedef struct {
    ZuiEventType type;
    ZuiWidget *target;
    union {
        struct { float x, y; uint32_t button; } mouse;
        struct { uint32_t key, sym; bool pressed; } keyboard;
        // ...
    };
} ZuiEvent;
```

#### Event Dispatch

1. **Hit testing**: Find the widget under the cursor
2. **Dispatch**: Call the appropriate vtable handler
3. **Propagation**: Bubble up to parent if not handled

```c
// Simplified dispatch logic
ZuiWidget *target = zui_widget_hit_test(root, x, y);
if (target && target->vtable->on_mouse_down) {
    target->vtable->on_mouse_down(target, x, y, button);
}
```

## Memory Management

ZUI uses explicit memory management:

```c
// Creation allocates
ZuiButton *button = zui_button_create("Click");

// Destruction frees
zui_widget_destroy((ZuiWidget *)button);
```

### Ownership Rules

1. **Window owns its widget tree** — Destroying a window destroys all children
2. **Widgets own their private data** — Strings, textures, etc. are copied/owned
3. **User data is borrowed** — Callbacks receive user data but don't own it

### Resource Lifecycle

```c
// Typical lifecycle
ZuiWindow *window = zui_window_create(...);  // Allocates window + EGL context
ZuiWidget *button = zui_button_new("...");   // Allocates widget
zui_widget_add_child(content, button);       // Transfers to tree

// ... use ...

zui_window_destroy(window);  // Frees everything
```

## Threading Model

ZUI is **single-threaded**. All ZUI calls must be made from the main thread.

For background work, use OS threads and synchronize with the main loop:

```c
// Worker thread
void *worker(void *arg) {
    // Do work...
    // Signal main thread (app-specific mechanism)
    return NULL;
}

// Main loop
while (zui_window_running(window)) {
    // Check for worker results
    if (work_ready) {
        update_ui_with_results();
    }
    
    zui_window_render(window);
    zui_poll_events();
}
```

## Performance Considerations

### Layout Invalidation

Layout is only recalculated when needed:

```c
void zui_widget_invalidate(ZuiWidget *widget)
{
    widget->needs_layout = true;
    if (widget->parent) {
        zui_widget_invalidate(widget->parent);
    }
}
```

### Clipping

The renderer uses GPU scissor testing for clipping:

```c
void zui_renderer_push_clip(ZuiRenderer *r, ZuiRect rect, float radius)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(rect.x, height - rect.y - rect.h, rect.width, rect.height);
}
```

### Batching

Future optimization: batch similar draw calls to reduce GPU state changes.

## Future Directions

### Platform Abstraction

The architecture supports future backends:

```
platform/
├── wayland/    # Current
├── x11/        # Future
├── windows/    # Future
└── macos/      # Future
```

### Renderer Backends

Potential future renderers:

- Vulkan
- Software rendering
- Metal (macOS)
- DirectX (Windows)

---

**See Also:**
- [Widget System](widgets/overview.md)
- [Event Handling](guides/events.md)
- [Custom Widgets](guides/custom-widgets.md)
