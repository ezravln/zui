# Panel

A container widget for organizing and laying out child widgets.

## Basic Usage

```c
// Create a panel
ZuiWidget *panel = zui_panel_new();

// Add children
ZuiWidget *label = zui_label_new("Hello");
ZuiWidget *button = zui_button_new("Click");

zui_panel_add_child(ZUI_PANEL(panel), label);
zui_panel_add_child(ZUI_PANEL(panel), button);
```

## Layout Direction

Panels can arrange children vertically or horizontally:

```c
// Vertical layout (default)
zui_panel_set_layout(ZUI_PANEL(panel), ZUI_LAYOUT_VERTICAL);

// Horizontal layout
zui_panel_set_layout(ZUI_PANEL(panel), ZUI_LAYOUT_HORIZONTAL);
```

```
Vertical:           Horizontal:
┌─────────┐         ┌─────────────────┐
│ Child 1 │         │ C1 │ C2 │ C3   │
├─────────┤         └─────────────────┘
│ Child 2 │
├─────────┤
│ Child 3 │
└─────────┘
```

## Spacing and Padding

```c
// Space between children
zui_panel_set_spacing(ZUI_PANEL(panel), 12);

// Padding around all children (top, right, bottom, left)
zui_panel_set_padding(ZUI_PANEL(panel), 20, 20, 20, 20);
```

```
┌────────────────────┐
│ ← padding →        │
│   ┌──────────┐     │
│   │ Child 1  │     │
│   └──────────┘     │
│      ↕ spacing     │
│   ┌──────────┐     │
│   │ Child 2  │     │
│   └──────────┘     │
│                    │
└────────────────────┘
```

## Alignment

Control how children are positioned within the panel:

```c
zui_panel_set_alignment(ZUI_PANEL(panel), 
    ZUI_ALIGN_CENTER,  // Main axis (direction of layout)
    ZUI_ALIGN_CENTER); // Cross axis (perpendicular)
```

### Alignment Options

| Value | Main Axis | Cross Axis |
|-------|-----------|------------|
| `ZUI_ALIGN_START` | Top/Left | Top/Left |
| `ZUI_ALIGN_CENTER` | Center | Center |
| `ZUI_ALIGN_END` | Bottom/Right | Bottom/Right |

### Examples

```
Main: START, Cross: CENTER     Main: CENTER, Cross: CENTER
┌──────────────────┐           ┌──────────────────┐
│ ┌──┐             │           │      ┌──┐        │
│ └──┘             │           │      └──┘        │
│ ┌──┐             │           │      ┌──┐        │
│ └──┘             │           │      └──┘        │
│                  │           │                  │
└──────────────────┘           └──────────────────┘

Main: END, Cross: END
┌──────────────────┐
│                  │
│             ┌──┐ │
│             └──┘ │
│             ┌──┐ │
│             └──┘ │
└──────────────────┘
```

## Sizing

### Fixed Size

```c
zui_panel_set_size(ZUI_PANEL(panel), 300, 200);
```

### Fill Parent

```c
// Fill width only
zui_panel_set_fill(ZUI_PANEL(panel), true, false);

// Fill both dimensions
zui_panel_set_fill(ZUI_PANEL(panel), true, true);
```

## Styling

### Background Color

```c
zui_panel_set_background(ZUI_PANEL(panel), ZUI_COLOR_HEX(0x2c3e50));
```

### Corner Radius

```c
zui_panel_set_corner_radius(ZUI_PANEL(panel), 12);
```

## Managing Children

### Adding Children

```c
zui_panel_add_child(ZUI_PANEL(panel), widget);
```

### Removing Children

```c
zui_panel_remove_child(ZUI_PANEL(panel), widget);
```

### Clearing All Children

```c
zui_panel_clear(ZUI_PANEL(panel));
```

## Examples

### Card Component

```c
ZuiWidget *create_card(const char *title, const char *content)
{
    ZuiWidget *card = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(card), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_padding(ZUI_PANEL(card), 16, 16, 16, 16);
    zui_panel_set_spacing(ZUI_PANEL(card), 8);
    zui_panel_set_background(ZUI_PANEL(card), ZUI_COLOR_HEX(0x34495e));
    zui_panel_set_corner_radius(ZUI_PANEL(card), 8);
    zui_panel_set_size(ZUI_PANEL(card), 280, 0);  // Fixed width, auto height
    
    // Title
    ZuiWidget *title_label = zui_label_new(title);
    zui_label_set_size(ZUI_LABEL(title_label), 18);
    zui_label_set_color(ZUI_LABEL(title_label), ZUI_COLOR_HEX(0xffffff));
    
    // Content
    ZuiWidget *content_label = zui_label_new(content);
    zui_label_set_size(ZUI_LABEL(content_label), 14);
    zui_label_set_color(ZUI_LABEL(content_label), ZUI_COLOR_HEX(0xbdc3c7));
    
    zui_panel_add_child(ZUI_PANEL(card), title_label);
    zui_panel_add_child(ZUI_PANEL(card), content_label);
    
    return card;
}
```

### Toolbar

```c
ZuiWidget *create_toolbar(void)
{
    ZuiWidget *toolbar = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(toolbar), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(toolbar), 8);
    zui_panel_set_padding(ZUI_PANEL(toolbar), 8, 12, 8, 12);
    zui_panel_set_background(ZUI_PANEL(toolbar), ZUI_COLOR_HEX(0x1a1a2e));
    zui_panel_set_fill(ZUI_PANEL(toolbar), true, false);
    
    // Add toolbar buttons
    zui_panel_add_child(ZUI_PANEL(toolbar), zui_button_new("New"));
    zui_panel_add_child(ZUI_PANEL(toolbar), zui_button_new("Open"));
    zui_panel_add_child(ZUI_PANEL(toolbar), zui_button_new("Save"));
    
    return toolbar;
}
```

### Form Layout

```c
ZuiWidget *create_form_row(const char *label_text, ZuiWidget *input)
{
    ZuiWidget *row = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(row), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(row), 12);
    zui_panel_set_alignment(ZUI_PANEL(row), ZUI_ALIGN_START, ZUI_ALIGN_CENTER);
    
    // Label
    ZuiWidget *label = zui_label_new(label_text);
    zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR_HEX(0xffffff));
    zui_widget_set_size(label, 100, 0);  // Fixed label width
    
    // Input fills remaining space
    zui_widget_set_fill(input, true, false);
    
    zui_panel_add_child(ZUI_PANEL(row), label);
    zui_panel_add_child(ZUI_PANEL(row), input);
    
    return row;
}

// Usage
ZuiWidget *form = zui_panel_new();
zui_panel_set_layout(ZUI_PANEL(form), ZUI_LAYOUT_VERTICAL);
zui_panel_set_spacing(ZUI_PANEL(form), 12);

zui_panel_add_child(ZUI_PANEL(form), 
    create_form_row("Name:", zui_textinput_new("Enter name")));
zui_panel_add_child(ZUI_PANEL(form), 
    create_form_row("Email:", zui_textinput_new("Enter email")));
```

### Nested Panels

```c
ZuiWidget *create_sidebar_layout(void)
{
    // Main horizontal container
    ZuiWidget *main = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(main), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_fill(ZUI_PANEL(main), true, true);
    
    // Sidebar
    ZuiWidget *sidebar = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(sidebar), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_background(ZUI_PANEL(sidebar), ZUI_COLOR_HEX(0x2c3e50));
    zui_widget_set_size(sidebar, 200, 0);
    zui_widget_set_fill(sidebar, false, true);
    
    // Content area
    ZuiWidget *content = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(content), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_fill(ZUI_PANEL(content), true, true);
    
    zui_panel_add_child(ZUI_PANEL(main), sidebar);
    zui_panel_add_child(ZUI_PANEL(main), content);
    
    return main;
}
```

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_panel_new()` | Create panel, returns `ZuiWidget *` |
| `zui_panel_create()` | Create panel, returns `ZuiPanel *` |

### Layout

| Function | Description |
|----------|-------------|
| `zui_panel_set_layout(panel, dir)` | Set layout direction |
| `zui_panel_set_spacing(panel, spacing)` | Set space between children |
| `zui_panel_set_padding(panel, t, r, b, l)` | Set padding |
| `zui_panel_set_alignment(panel, main, cross)` | Set alignment |

### Sizing

| Function | Description |
|----------|-------------|
| `zui_panel_set_size(panel, w, h)` | Set fixed size |
| `zui_panel_set_fill(panel, w, h)` | Set fill behavior |

### Styling

| Function | Description |
|----------|-------------|
| `zui_panel_set_background(panel, color)` | Set background color |
| `zui_panel_set_corner_radius(panel, radius)` | Set corner radius |

### Children

| Function | Description |
|----------|-------------|
| `zui_panel_add_child(panel, widget)` | Add child widget |
| `zui_panel_remove_child(panel, widget)` | Remove child widget |
| `zui_panel_clear(panel)` | Remove all children |

### Conversion

| Function | Description |
|----------|-------------|
| `zui_panel_as_widget(panel)` | Convert to `ZuiWidget *` |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_PANEL(w)` | Check if widget is a panel |
| `ZUI_PANEL(w)` | Downcast to `ZuiPanel *` (or NULL) |

---

**See Also:** [Layout Guide](../guides/layout.md) · [ScrollView](scrollview.md) · [SplitView](splitview.md)
