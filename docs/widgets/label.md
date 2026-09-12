# Label

A text display widget for showing static or dynamic text.

## Basic Usage

```c
// Create a label
ZuiWidget *label = zui_label_new("Hello, World!");

// Add to parent
zui_panel_add_child(ZUI_PANEL(panel), label);
```

## Updating Text

```c
ZuiWidget *status = zui_label_new("Ready");

// Later...
zui_label_set_text(ZUI_LABEL(status), "Processing...");

// Dynamic text
char buffer[64];
snprintf(buffer, sizeof(buffer), "Items: %d", count);
zui_label_set_text(ZUI_LABEL(status), buffer);
```

## Styling

### Text Color

```c
// Using hex color
zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR_HEX(0xffffff));

// Using RGBA
zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR(1.0f, 0.5f, 0.0f, 1.0f));

// Semi-transparent
zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR(1, 1, 1, 0.6f));
```

### Text Size

```c
// Set font size in pixels
zui_label_set_size(ZUI_LABEL(label), 24);  // 24px
```

### Custom Font

```c
// Load a font
ZuiFont *heading_font = zui_font_load("fonts/Roboto-Bold.ttf", 28);

// Apply to label
zui_label_set_font(ZUI_LABEL(label), heading_font);
```

## Examples

### Title and Subtitle

```c
ZuiWidget *create_header(const char *title, const char *subtitle)
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 4);
    
    // Title
    ZuiWidget *title_label = zui_label_new(title);
    zui_label_set_size(ZUI_LABEL(title_label), 24);
    zui_label_set_color(ZUI_LABEL(title_label), ZUI_COLOR_HEX(0xffffff));
    
    // Subtitle
    ZuiWidget *sub_label = zui_label_new(subtitle);
    zui_label_set_size(ZUI_LABEL(sub_label), 14);
    zui_label_set_color(ZUI_LABEL(sub_label), ZUI_COLOR_HEX(0x888888));
    
    zui_panel_add_child(ZUI_PANEL(container), title_label);
    zui_panel_add_child(ZUI_PANEL(container), sub_label);
    
    return container;
}
```

### Status Indicator

```c
typedef enum { STATUS_OK, STATUS_WARNING, STATUS_ERROR } Status;

void update_status(ZuiWidget *label, Status status, const char *message)
{
    zui_label_set_text(ZUI_LABEL(label), message);
    
    ZuiColor color;
    switch (status) {
        case STATUS_OK:      color = ZUI_COLOR_HEX(0x2ecc71); break;
        case STATUS_WARNING: color = ZUI_COLOR_HEX(0xf1c40f); break;
        case STATUS_ERROR:   color = ZUI_COLOR_HEX(0xe74c3c); break;
    }
    zui_label_set_color(ZUI_LABEL(label), color);
}
```

### Counter Display

```c
typedef struct {
    ZuiWidget *label;
    int value;
} Counter;

void counter_init(Counter *c)
{
    c->value = 0;
    c->label = zui_label_new("0");
    zui_label_set_size(ZUI_LABEL(c->label), 48);
    zui_label_set_color(ZUI_LABEL(c->label), ZUI_COLOR_HEX(0x3498db));
}

void counter_set(Counter *c, int value)
{
    c->value = value;
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    zui_label_set_text(ZUI_LABEL(c->label), buf);
}

void counter_increment(Counter *c)
{
    counter_set(c, c->value + 1);
}
```

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_label_new(text)` | Create label, returns `ZuiWidget *` |
| `zui_label_create(text)` | Create label, returns `ZuiLabel *` |

### Configuration

| Function | Description |
|----------|-------------|
| `zui_label_set_text(label, text)` | Set label text |
| `zui_label_set_color(label, color)` | Set text color |
| `zui_label_set_size(label, size)` | Set font size in pixels |
| `zui_label_set_font(label, font)` | Set custom font |

### Accessors

| Function | Description |
|----------|-------------|
| `zui_label_get_text(label)` | Get label text |
| `zui_label_as_widget(label)` | Convert to `ZuiWidget *` |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_LABEL(w)` | Check if widget is a label |
| `ZUI_LABEL(w)` | Downcast to `ZuiLabel *` (or NULL) |

## Notes

- Labels automatically resize to fit their text content
- For multi-line text, use [TextArea](textarea.md) with `readonly` mode
- Text is rendered using FreeType for high-quality anti-aliased fonts

---

**See Also:** [Button](button.md) · [TextArea](textarea.md) · [Widget Overview](overview.md)
