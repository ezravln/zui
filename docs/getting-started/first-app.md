# Your First App

In this tutorial, we'll build a simple counter application to learn ZUI fundamentals.

## What We'll Build

A counter app with:
- A label showing the current count
- Increment and decrement buttons
- A reset button

## Project Setup

Create a new directory and files:

```bash
mkdir counter-app
cd counter-app
```

Create `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(counter_app C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZUI REQUIRED zui)

add_executable(counter main.c)
target_include_directories(counter PRIVATE ${ZUI_INCLUDE_DIRS})
target_link_libraries(counter ${ZUI_LIBRARIES})
```

## The Code

Create `main.c`:

```c
#include <zui/zui.h>
#include <stdio.h>

// Application state
typedef struct {
    int count;
    ZuiWidget *count_label;
} AppState;

static AppState app = {0};

// Update the count display
static void update_display(void)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", app.count);
    zui_label_set_text(ZUI_LABEL(app.count_label), buf);
}

// Button callbacks
static void on_increment(ZuiWidget *widget, void *data)
{
    (void)widget; (void)data;
    app.count++;
    update_display();
}

static void on_decrement(ZuiWidget *widget, void *data)
{
    (void)widget; (void)data;
    app.count--;
    update_display();
}

static void on_reset(ZuiWidget *widget, void *data)
{
    (void)widget; (void)data;
    app.count = 0;
    update_display();
}

// Create a styled button
static ZuiWidget *create_button(const char *text, ZuiColor color, 
                                 ZuiClickCallback callback)
{
    ZuiWidget *btn = zui_button_new(text);
    zui_button_set_colors(ZUI_BUTTON(btn), 
        color,                              // normal
        ZUI_COLOR(color.r * 1.1f, color.g * 1.1f, color.b * 1.1f, 1), // hover
        ZUI_COLOR(color.r * 0.9f, color.g * 0.9f, color.b * 0.9f, 1)  // pressed
    );
    zui_button_set_text_color(ZUI_BUTTON(btn), ZUI_COLOR_HEX(0xffffff));
    zui_button_set_corner_radius(ZUI_BUTTON(btn), 8);
    zui_widget_set_size(btn, 80, 40);
    zui_button_on_click(ZUI_BUTTON(btn), callback, NULL);
    return btn;
}

static ZuiWidget *create_ui(void)
{
    // Main container
    ZuiWidget *main_panel = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(main_panel), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_alignment(ZUI_PANEL(main_panel), ZUI_ALIGN_CENTER, ZUI_ALIGN_CENTER);
    zui_panel_set_spacing(ZUI_PANEL(main_panel), 24);
    zui_panel_set_fill(ZUI_PANEL(main_panel), true, true);
    
    // Title
    ZuiWidget *title = zui_label_new("Counter");
    zui_label_set_size(ZUI_LABEL(title), 28);
    zui_label_set_color(ZUI_LABEL(title), ZUI_COLOR_HEX(0xffffff));
    
    // Count display
    app.count_label = zui_label_new("0");
    zui_label_set_size(ZUI_LABEL(app.count_label), 72);
    zui_label_set_color(ZUI_LABEL(app.count_label), ZUI_COLOR_HEX(0x3498db));
    
    // Button container (horizontal layout)
    ZuiWidget *button_panel = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(button_panel), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(button_panel), 12);
    zui_panel_set_alignment(ZUI_PANEL(button_panel), ZUI_ALIGN_CENTER, ZUI_ALIGN_CENTER);
    
    // Create buttons
    ZuiWidget *dec_btn = create_button("-", ZUI_COLOR_HEX(0xe74c3c), on_decrement);
    ZuiWidget *reset_btn = create_button("Reset", ZUI_COLOR_HEX(0x7f8c8d), on_reset);
    ZuiWidget *inc_btn = create_button("+", ZUI_COLOR_HEX(0x2ecc71), on_increment);
    
    // Add buttons to button panel
    zui_panel_add_child(ZUI_PANEL(button_panel), dec_btn);
    zui_panel_add_child(ZUI_PANEL(button_panel), reset_btn);
    zui_panel_add_child(ZUI_PANEL(button_panel), inc_btn);
    
    // Assemble main panel
    zui_panel_add_child(ZUI_PANEL(main_panel), title);
    zui_panel_add_child(ZUI_PANEL(main_panel), app.count_label);
    zui_panel_add_child(ZUI_PANEL(main_panel), button_panel);
    
    return main_panel;
}

int main(void)
{
    if (!zui_init()) {
        fprintf(stderr, "Failed to initialize ZUI\n");
        return 1;
    }
    
    // Create window
    ZuiWindow *window = zui_window_create(400, 350, "Counter");
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }
    
    // Configure window
    zui_window_set_min_size(window, 300, 250);
    zui_window_set_background_color(window, ZUI_COLOR_HEX(0x1a1a2e));
    zui_window_set_corner_radius(window, 12);
    
    // Build UI
    ZuiWidget *ui = create_ui();
    zui_widget_add_child(zui_window_content(window), ui);
    
    zui_window_show(window);
    
    // Main loop
    while (zui_window_running(window)) {
        zui_window_render(window);
        zui_poll_events();
    }
    
    zui_window_destroy(window);
    zui_shutdown();
    
    return 0;
}
```

## Build and Run

```bash
cmake -B build
cmake --build build
./build/counter
```

## Code Breakdown

### Application State

```c
typedef struct {
    int count;
    ZuiWidget *count_label;
} AppState;

static AppState app = {0};
```

We use a simple struct to hold our application state. The `count_label` reference lets us update the display when the count changes.

### Creating Widgets

```c
ZuiWidget *btn = zui_button_new(text);
```

The `zui_*_new()` functions create widgets and return `ZuiWidget *`. This unified type makes it easy to work with widgets generically.

### Downcasting

```c
zui_button_set_colors(ZUI_BUTTON(btn), ...);
```

When you need widget-specific functions, use the `ZUI_*()` macros to downcast. These macros are type-safe and return `NULL` if the widget type doesn't match.

### Layout

```c
zui_panel_set_layout(ZUI_PANEL(panel), ZUI_LAYOUT_VERTICAL);
zui_panel_set_spacing(ZUI_PANEL(panel), 24);
zui_panel_set_alignment(ZUI_PANEL(panel), ZUI_ALIGN_CENTER, ZUI_ALIGN_CENTER);
```

Panels arrange their children either vertically or horizontally. Use spacing and alignment to control the layout.

### Event Handling

```c
zui_button_on_click(ZUI_BUTTON(btn), on_increment, NULL);
```

Callbacks receive the widget and optional user data. Use them to respond to user interactions.

## Exercises

Try extending the counter app:

1. **Add keyboard shortcuts**: Increment with `+` key, decrement with `-`
2. **Add bounds**: Prevent the count from going below 0
3. **Add persistence**: Save/load the count to a file
4. **Add animation**: Animate the count change

## What's Next

- **[Widget Overview](../widgets/overview.md)** — Learn about all available widgets
- **[Layout System](../guides/layout.md)** — Master complex layouts
- **[Event Handling](../guides/events.md)** — Deep dive into events
- **[Styling Guide](../guides/styling.md)** — Make your app beautiful

---

**Next:** [Widget Overview →](../widgets/overview.md)
