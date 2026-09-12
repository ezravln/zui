# Slider

A draggable slider widget for selecting numeric values within a range.

## Basic Usage

```c
// Create a slider (min, max, initial value)
ZuiWidget *slider = zui_slider_new(0, 100, 50);

// Add to parent
zui_panel_add_child(ZUI_PANEL(panel), slider);
```

## Handling Value Changes

```c
void on_value_change(ZuiSlider *slider, float value, void *data)
{
    printf("Value: %.1f\n", value);
}

ZuiWidget *slider = zui_slider_new(0, 100, 50);
zui_slider_on_change(ZUI_SLIDER(slider), on_value_change, NULL);
```

## Getting and Setting Values

```c
// Set value programmatically
zui_slider_set_value(ZUI_SLIDER(slider), 75);

// Get current value
float value = zui_slider_get_value(ZUI_SLIDER(slider));
```

## Styling

### Colors

```c
// Track color (background)
zui_slider_set_track_color(ZUI_SLIDER(slider), ZUI_COLOR_HEX(0x2c3e50));

// Fill color (active portion)
zui_slider_set_fill_color(ZUI_SLIDER(slider), ZUI_COLOR_HEX(0x3498db));

// Thumb colors (the draggable handle)
zui_slider_set_thumb_colors(ZUI_SLIDER(slider),
    ZUI_COLOR_HEX(0xffffff),  // Normal
    ZUI_COLOR_HEX(0xecf0f1),  // Hover
    ZUI_COLOR_HEX(0xbdc3c7)); // Dragging
```

### Thumb Size

```c
zui_slider_set_thumb_radius(ZUI_SLIDER(slider), 8);
```

### Size

```c
// Fixed width
zui_widget_set_size(slider, 200, 20);

// Fill container width
zui_widget_set_fill(slider, true, false);
```

## Examples

### Volume Control

```c
typedef struct {
    ZuiWidget *slider;
    ZuiWidget *label;
} VolumeControl;

void on_volume_change(ZuiSlider *slider, float value, void *data)
{
    VolumeControl *vc = (VolumeControl *)data;
    
    char buf[32];
    snprintf(buf, sizeof(buf), "Volume: %.0f%%", value);
    zui_label_set_text(ZUI_LABEL(vc->label), buf);
    
    // Actually set the volume
    // set_system_volume(value / 100.0f);
}

ZuiWidget *create_volume_control(VolumeControl *vc)
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 8);
    
    vc->label = zui_label_new("Volume: 50%");
    zui_label_set_color(ZUI_LABEL(vc->label), ZUI_COLOR_HEX(0xffffff));
    
    vc->slider = zui_slider_new(0, 100, 50);
    zui_widget_set_fill(vc->slider, true, false);
    zui_slider_set_fill_color(ZUI_SLIDER(vc->slider), ZUI_COLOR_HEX(0x2ecc71));
    zui_slider_on_change(ZUI_SLIDER(vc->slider), on_volume_change, vc);
    
    zui_panel_add_child(ZUI_PANEL(container), vc->label);
    zui_panel_add_child(ZUI_PANEL(container), vc->slider);
    
    return container;
}
```

### Color Picker (RGB Sliders)

```c
typedef struct {
    ZuiWidget *r_slider;
    ZuiWidget *g_slider;
    ZuiWidget *b_slider;
    ZuiWidget *preview;
} ColorPicker;

void update_color(ZuiSlider *slider, float value, void *data)
{
    ColorPicker *cp = (ColorPicker *)data;
    
    float r = zui_slider_get_value(ZUI_SLIDER(cp->r_slider)) / 255.0f;
    float g = zui_slider_get_value(ZUI_SLIDER(cp->g_slider)) / 255.0f;
    float b = zui_slider_get_value(ZUI_SLIDER(cp->b_slider)) / 255.0f;
    
    zui_widget_set_background(cp->preview, ZUI_COLOR(r, g, b, 1.0f));
}

ZuiWidget *create_color_slider(const char *label, ZuiColor fill_color,
                                ZuiSliderCallback cb, void *data)
{
    ZuiWidget *row = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(row), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(row), 8);
    zui_panel_set_alignment(ZUI_PANEL(row), ZUI_ALIGN_START, ZUI_ALIGN_CENTER);
    
    ZuiWidget *lbl = zui_label_new(label);
    zui_label_set_color(ZUI_LABEL(lbl), ZUI_COLOR_HEX(0xffffff));
    zui_widget_set_size(lbl, 20, 0);
    
    ZuiWidget *slider = zui_slider_new(0, 255, 128);
    zui_widget_set_fill(slider, true, false);
    zui_slider_set_fill_color(ZUI_SLIDER(slider), fill_color);
    zui_slider_on_change(ZUI_SLIDER(slider), cb, data);
    
    zui_panel_add_child(ZUI_PANEL(row), lbl);
    zui_panel_add_child(ZUI_PANEL(row), slider);
    
    return row;
}

ZuiWidget *create_color_picker(ColorPicker *cp)
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 12);
    zui_panel_set_size(ZUI_PANEL(container), 300, 0);
    
    // Preview box
    cp->preview = zui_panel_new();
    zui_panel_set_size(ZUI_PANEL(cp->preview), 0, 60);
    zui_panel_set_fill(ZUI_PANEL(cp->preview), true, false);
    zui_panel_set_corner_radius(ZUI_PANEL(cp->preview), 8);
    
    // RGB sliders
    ZuiWidget *r_row = create_color_slider("R", ZUI_COLOR_HEX(0xe74c3c), update_color, cp);
    ZuiWidget *g_row = create_color_slider("G", ZUI_COLOR_HEX(0x2ecc71), update_color, cp);
    ZuiWidget *b_row = create_color_slider("B", ZUI_COLOR_HEX(0x3498db), update_color, cp);
    
    // Store slider references
    cp->r_slider = zui_widget_get_child(r_row, 1);
    cp->g_slider = zui_widget_get_child(g_row, 1);
    cp->b_slider = zui_widget_get_child(b_row, 1);
    
    zui_panel_add_child(ZUI_PANEL(container), cp->preview);
    zui_panel_add_child(ZUI_PANEL(container), r_row);
    zui_panel_add_child(ZUI_PANEL(container), g_row);
    zui_panel_add_child(ZUI_PANEL(container), b_row);
    
    // Initial color
    update_color(NULL, 0, cp);
    
    return container;
}
```

### Settings Panel

```c
ZuiWidget *create_setting_row(const char *name, float min, float max, 
                               float value, ZuiSliderCallback cb, void *data)
{
    ZuiWidget *row = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(row), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(row), 12);
    zui_panel_set_fill(ZUI_PANEL(row), true, false);
    
    ZuiWidget *label = zui_label_new(name);
    zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR_HEX(0xffffff));
    zui_widget_set_size(label, 120, 0);
    
    ZuiWidget *slider = zui_slider_new(min, max, value);
    zui_widget_set_fill(slider, true, false);
    zui_slider_on_change(ZUI_SLIDER(slider), cb, data);
    
    zui_panel_add_child(ZUI_PANEL(row), label);
    zui_panel_add_child(ZUI_PANEL(row), slider);
    
    return row;
}
```

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_slider_new(min, max, value)` | Create slider, returns `ZuiWidget *` |
| `zui_slider_create(min, max, value)` | Create slider, returns `ZuiSlider *` |

### Value

| Function | Description |
|----------|-------------|
| `zui_slider_set_value(slider, value)` | Set current value |
| `zui_slider_get_value(slider)` | Get current value |

### Styling

| Function | Description |
|----------|-------------|
| `zui_slider_set_track_color(slider, color)` | Track background color |
| `zui_slider_set_fill_color(slider, color)` | Active fill color |
| `zui_slider_set_thumb_colors(slider, normal, hover, drag)` | Thumb colors |
| `zui_slider_set_thumb_radius(slider, radius)` | Thumb size |

### Events

| Function | Description |
|----------|-------------|
| `zui_slider_on_change(slider, callback, data)` | Value change handler |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_SLIDER(w)` | Check if widget is a slider |
| `ZUI_SLIDER(w)` | Downcast to `ZuiSlider *` (or NULL) |

---

**See Also:** [ProgressBar](progressbar.md) · [TextInput](textinput.md) · [Widget Overview](overview.md)
