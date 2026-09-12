# Button

A clickable button widget that triggers actions when pressed.

## Basic Usage

```c
// Create a button
ZuiWidget *button = zui_button_new("Click Me");

// Add to parent
zui_panel_add_child(ZUI_PANEL(panel), button);
```

## Handling Clicks

```c
void on_click(ZuiWidget *widget, void *data)
{
    printf("Button clicked!\n");
}

ZuiWidget *button = zui_button_new("Submit");
zui_button_on_click(ZUI_BUTTON(button), on_click, NULL);
```

With user data:

```c
typedef struct {
    int click_count;
} AppState;

void on_click(ZuiWidget *widget, void *data)
{
    AppState *state = (AppState *)data;
    state->click_count++;
    printf("Clicked %d times\n", state->click_count);
}

AppState state = {0};
zui_button_on_click(ZUI_BUTTON(button), on_click, &state);
```

## Styling

### Colors

```c
zui_button_set_colors(ZUI_BUTTON(button),
    ZUI_COLOR_HEX(0x3498db),  // Normal state
    ZUI_COLOR_HEX(0x5dade2),  // Hover state
    ZUI_COLOR_HEX(0x2980b9)); // Pressed state

zui_button_set_text_color(ZUI_BUTTON(button), ZUI_COLOR_HEX(0xffffff));
```

### Corner Radius

```c
zui_button_set_corner_radius(ZUI_BUTTON(button), 8);
```

### Size

```c
// Fixed size
zui_widget_set_size(button, 120, 40);

// Fill parent width
zui_widget_set_fill(button, true, false);
```

### Cursor

```c
zui_set_cursor(button, ZUI_CURSOR_POINTER);
```

## Button with Icon

```c
// Load icon
ZuiIconSource *icon = zui_icon_load("icons/save.svg");

// Set icon
zui_button_set_icon(ZUI_BUTTON(button), icon, 16);  // 16px icon size
zui_button_set_icon_spacing(ZUI_BUTTON(button), 8); // 8px between icon and text
```

## Examples

### Primary Button

```c
ZuiWidget *create_primary_button(const char *text)
{
    ZuiWidget *btn = zui_button_new(text);
    zui_button_set_colors(ZUI_BUTTON(btn),
        ZUI_COLOR_HEX(0x3498db),
        ZUI_COLOR_HEX(0x5dade2),
        ZUI_COLOR_HEX(0x2980b9));
    zui_button_set_text_color(ZUI_BUTTON(btn), ZUI_COLOR_HEX(0xffffff));
    zui_button_set_corner_radius(ZUI_BUTTON(btn), 6);
    zui_widget_set_size(btn, 120, 40);
    zui_set_cursor(btn, ZUI_CURSOR_POINTER);
    return btn;
}
```

### Danger Button

```c
ZuiWidget *create_danger_button(const char *text)
{
    ZuiWidget *btn = zui_button_new(text);
    zui_button_set_colors(ZUI_BUTTON(btn),
        ZUI_COLOR_HEX(0xe74c3c),
        ZUI_COLOR_HEX(0xec7063),
        ZUI_COLOR_HEX(0xc0392b));
    zui_button_set_text_color(ZUI_BUTTON(btn), ZUI_COLOR_HEX(0xffffff));
    zui_button_set_corner_radius(ZUI_BUTTON(btn), 6);
    return btn;
}
```

### Ghost Button (Outline)

```c
ZuiWidget *create_ghost_button(const char *text)
{
    ZuiWidget *btn = zui_button_new(text);
    zui_button_set_colors(ZUI_BUTTON(btn),
        ZUI_COLOR(0, 0, 0, 0),           // Transparent
        ZUI_COLOR(1, 1, 1, 0.1f),        // Subtle hover
        ZUI_COLOR(1, 1, 1, 0.2f));       // Subtle pressed
    zui_button_set_text_color(ZUI_BUTTON(btn), ZUI_COLOR_HEX(0xffffff));
    return btn;
}
```

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_button_new(text)` | Create button, returns `ZuiWidget *` |
| `zui_button_create(text)` | Create button, returns `ZuiButton *` |

### Configuration

| Function | Description |
|----------|-------------|
| `zui_button_set_text(btn, text)` | Set button text |
| `zui_button_set_colors(btn, normal, hover, pressed)` | Set state colors |
| `zui_button_set_text_color(btn, color)` | Set text color |
| `zui_button_set_corner_radius(btn, radius)` | Set corner radius |
| `zui_button_set_icon(btn, icon, size)` | Set button icon |
| `zui_button_set_icon_spacing(btn, spacing)` | Set icon-text spacing |

### Events

| Function | Description |
|----------|-------------|
| `zui_button_on_click(btn, callback, data)` | Set click handler |

### Accessors

| Function | Description |
|----------|-------------|
| `zui_button_get_text(btn)` | Get button text |
| `zui_button_as_widget(btn)` | Convert to `ZuiWidget *` |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_BUTTON(w)` | Check if widget is a button |
| `ZUI_BUTTON(w)` | Downcast to `ZuiButton *` (or NULL) |

---

**See Also:** [Label](label.md) · [Panel](panel.md) · [Widget Overview](overview.md)
