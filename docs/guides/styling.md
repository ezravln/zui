# Styling & Theming

This guide covers how to style ZUI widgets and create consistent visual themes.

## Colors

### Color Formats

```c
// RGBA (0.0 - 1.0)
ZuiColor red = ZUI_COLOR(1.0f, 0.0f, 0.0f, 1.0f);

// RGB (alpha = 1.0)
ZuiColor white = ZUI_COLOR_RGB(1.0f, 1.0f, 1.0f);

// Hex (0xRRGGBB)
ZuiColor blue = ZUI_COLOR_HEX(0x3498db);

// Semi-transparent
ZuiColor overlay = ZUI_COLOR(0, 0, 0, 0.5f);

// Fully transparent
ZuiColor transparent = ZUI_COLOR(0, 0, 0, 0);
```

### Common Colors

```c
// Primary colors
#define PRIMARY       ZUI_COLOR_HEX(0x3498db)
#define PRIMARY_LIGHT ZUI_COLOR_HEX(0x5dade2)
#define PRIMARY_DARK  ZUI_COLOR_HEX(0x2980b9)

// Semantic colors
#define SUCCESS ZUI_COLOR_HEX(0x2ecc71)
#define WARNING ZUI_COLOR_HEX(0xf1c40f)
#define DANGER  ZUI_COLOR_HEX(0xe74c3c)
#define INFO    ZUI_COLOR_HEX(0x3498db)

// Neutrals
#define GRAY_900 ZUI_COLOR_HEX(0x1a1a2e)
#define GRAY_800 ZUI_COLOR_HEX(0x2c3e50)
#define GRAY_700 ZUI_COLOR_HEX(0x34495e)
#define GRAY_600 ZUI_COLOR_HEX(0x7f8c8d)
#define GRAY_500 ZUI_COLOR_HEX(0x95a5a6)
#define GRAY_400 ZUI_COLOR_HEX(0xbdc3c7)
#define GRAY_300 ZUI_COLOR_HEX(0xdfe6e9)
#define GRAY_100 ZUI_COLOR_HEX(0xecf0f1)
#define WHITE    ZUI_COLOR_HEX(0xffffff)
```

## Backgrounds

### Solid Color

```c
zui_widget_set_background(widget, ZUI_COLOR_HEX(0x2c3e50));
zui_panel_set_background(ZUI_PANEL(panel), ZUI_COLOR_HEX(0x34495e));
zui_window_set_background_color(window, ZUI_COLOR_HEX(0x1a1a2e));
```

### Transparent

```c
zui_widget_set_background(widget, ZUI_COLOR(0, 0, 0, 0));
```

## Corner Radius

```c
// Rounded corners
zui_widget_set_corner_radius(widget, 8);
zui_button_set_corner_radius(ZUI_BUTTON(button), 6);
zui_panel_set_corner_radius(ZUI_PANEL(panel), 12);
zui_window_set_corner_radius(window, 16);

// Pill shape (for buttons)
zui_widget_set_size(button, 100, 40);
zui_button_set_corner_radius(ZUI_BUTTON(button), 20);  // height / 2
```

## Typography

### Font Sizes

```c
// Heading sizes
zui_label_set_size(ZUI_LABEL(h1), 32);
zui_label_set_size(ZUI_LABEL(h2), 24);
zui_label_set_size(ZUI_LABEL(h3), 20);

// Body text
zui_label_set_size(ZUI_LABEL(body), 16);

// Small text
zui_label_set_size(ZUI_LABEL(caption), 12);
```

### Custom Fonts

```c
// Load fonts
ZuiFont *heading_font = zui_font_load("fonts/Roboto-Bold.ttf", 24);
ZuiFont *body_font = zui_font_load("fonts/Roboto-Regular.ttf", 16);

// Apply to labels
zui_label_set_font(ZUI_LABEL(title), heading_font);
zui_label_set_font(ZUI_LABEL(paragraph), body_font);
```

### Text Colors

```c
// Primary text
zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR_HEX(0xffffff));

// Secondary text
zui_label_set_color(ZUI_LABEL(subtitle), ZUI_COLOR_HEX(0x95a5a6));

// Muted text
zui_label_set_color(ZUI_LABEL(caption), ZUI_COLOR_HEX(0x7f8c8d));
```

## Button Styles

### Primary Button

```c
ZuiWidget *create_primary_button(const char *text)
{
    ZuiWidget *btn = zui_button_new(text);
    zui_button_set_colors(ZUI_BUTTON(btn),
        ZUI_COLOR_HEX(0x3498db),  // Normal
        ZUI_COLOR_HEX(0x5dade2),  // Hover
        ZUI_COLOR_HEX(0x2980b9)); // Pressed
    zui_button_set_text_color(ZUI_BUTTON(btn), ZUI_COLOR_HEX(0xffffff));
    zui_button_set_corner_radius(ZUI_BUTTON(btn), 6);
    zui_set_cursor(btn, ZUI_CURSOR_POINTER);
    return btn;
}
```

### Secondary Button

```c
ZuiWidget *create_secondary_button(const char *text)
{
    ZuiWidget *btn = zui_button_new(text);
    zui_button_set_colors(ZUI_BUTTON(btn),
        ZUI_COLOR_HEX(0x34495e),
        ZUI_COLOR_HEX(0x3d566e),
        ZUI_COLOR_HEX(0x2c3e50));
    zui_button_set_text_color(ZUI_BUTTON(btn), ZUI_COLOR_HEX(0xffffff));
    zui_button_set_corner_radius(ZUI_BUTTON(btn), 6);
    return btn;
}
```

### Ghost Button

```c
ZuiWidget *create_ghost_button(const char *text)
{
    ZuiWidget *btn = zui_button_new(text);
    zui_button_set_colors(ZUI_BUTTON(btn),
        ZUI_COLOR(0, 0, 0, 0),       // Transparent
        ZUI_COLOR(1, 1, 1, 0.1f),    // Subtle hover
        ZUI_COLOR(1, 1, 1, 0.2f));   // Subtle pressed
    zui_button_set_text_color(ZUI_BUTTON(btn), ZUI_COLOR_HEX(0x3498db));
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

## Input Styles

### Text Input

```c
void style_text_input(ZuiWidget *input)
{
    zui_textinput_set_bg_color(ZUI_TEXTINPUT(input), ZUI_COLOR_HEX(0x2c3e50));
    zui_textinput_set_text_color(ZUI_TEXTINPUT(input), ZUI_COLOR_HEX(0xffffff));
    zui_textinput_set_placeholder_color(ZUI_TEXTINPUT(input), ZUI_COLOR_HEX(0x7f8c8d));
    zui_textinput_set_border_color(ZUI_TEXTINPUT(input), ZUI_COLOR_HEX(0x34495e));
    zui_textinput_set_corner_radius(ZUI_TEXTINPUT(input), 6);
    zui_textinput_set_padding(ZUI_TEXTINPUT(input), 12);
}
```

### Slider

```c
void style_slider(ZuiWidget *slider)
{
    zui_slider_set_track_color(ZUI_SLIDER(slider), ZUI_COLOR_HEX(0x34495e));
    zui_slider_set_fill_color(ZUI_SLIDER(slider), ZUI_COLOR_HEX(0x3498db));
    zui_slider_set_thumb_colors(ZUI_SLIDER(slider),
        ZUI_COLOR_HEX(0xffffff),
        ZUI_COLOR_HEX(0xecf0f1),
        ZUI_COLOR_HEX(0xbdc3c7));
    zui_slider_set_thumb_radius(ZUI_SLIDER(slider), 8);
}
```

## Creating a Theme

### Theme Structure

```c
typedef struct {
    // Backgrounds
    ZuiColor bg_primary;
    ZuiColor bg_secondary;
    ZuiColor bg_tertiary;
    
    // Text
    ZuiColor text_primary;
    ZuiColor text_secondary;
    ZuiColor text_muted;
    
    // Accent
    ZuiColor accent;
    ZuiColor accent_hover;
    ZuiColor accent_pressed;
    
    // Semantic
    ZuiColor success;
    ZuiColor warning;
    ZuiColor danger;
    
    // Borders
    ZuiColor border;
    float corner_radius;
    
    // Typography
    ZuiFont *font_heading;
    ZuiFont *font_body;
    float font_size_lg;
    float font_size_md;
    float font_size_sm;
    
    // Spacing
    float spacing_sm;
    float spacing_md;
    float spacing_lg;
} Theme;
```

### Dark Theme

```c
Theme dark_theme = {
    .bg_primary = ZUI_COLOR_HEX(0x1a1a2e),
    .bg_secondary = ZUI_COLOR_HEX(0x2c3e50),
    .bg_tertiary = ZUI_COLOR_HEX(0x34495e),
    
    .text_primary = ZUI_COLOR_HEX(0xffffff),
    .text_secondary = ZUI_COLOR_HEX(0xbdc3c7),
    .text_muted = ZUI_COLOR_HEX(0x7f8c8d),
    
    .accent = ZUI_COLOR_HEX(0x3498db),
    .accent_hover = ZUI_COLOR_HEX(0x5dade2),
    .accent_pressed = ZUI_COLOR_HEX(0x2980b9),
    
    .success = ZUI_COLOR_HEX(0x2ecc71),
    .warning = ZUI_COLOR_HEX(0xf1c40f),
    .danger = ZUI_COLOR_HEX(0xe74c3c),
    
    .border = ZUI_COLOR_HEX(0x34495e),
    .corner_radius = 8,
    
    .spacing_sm = 8,
    .spacing_md = 16,
    .spacing_lg = 24,
};
```

### Light Theme

```c
Theme light_theme = {
    .bg_primary = ZUI_COLOR_HEX(0xffffff),
    .bg_secondary = ZUI_COLOR_HEX(0xf5f5f5),
    .bg_tertiary = ZUI_COLOR_HEX(0xeeeeee),
    
    .text_primary = ZUI_COLOR_HEX(0x212121),
    .text_secondary = ZUI_COLOR_HEX(0x757575),
    .text_muted = ZUI_COLOR_HEX(0x9e9e9e),
    
    .accent = ZUI_COLOR_HEX(0x2196f3),
    .accent_hover = ZUI_COLOR_HEX(0x42a5f5),
    .accent_pressed = ZUI_COLOR_HEX(0x1976d2),
    
    .success = ZUI_COLOR_HEX(0x4caf50),
    .warning = ZUI_COLOR_HEX(0xff9800),
    .danger = ZUI_COLOR_HEX(0xf44336),
    
    .border = ZUI_COLOR_HEX(0xe0e0e0),
    .corner_radius = 8,
    
    .spacing_sm = 8,
    .spacing_md = 16,
    .spacing_lg = 24,
};
```

### Applying Theme

```c
void apply_theme(Theme *t, ZuiWindow *window, ZuiWidget *root)
{
    // Window
    zui_window_set_background_color(window, t->bg_primary);
    
    // Apply to all widgets recursively
    apply_theme_to_widget(t, root);
}

void apply_theme_to_widget(Theme *t, ZuiWidget *widget)
{
    if (ZUI_IS_BUTTON(widget)) {
        zui_button_set_colors(ZUI_BUTTON(widget),
            t->accent, t->accent_hover, t->accent_pressed);
        zui_button_set_text_color(ZUI_BUTTON(widget), t->text_primary);
        zui_button_set_corner_radius(ZUI_BUTTON(widget), t->corner_radius);
    }
    else if (ZUI_IS_LABEL(widget)) {
        zui_label_set_color(ZUI_LABEL(widget), t->text_primary);
    }
    else if (ZUI_IS_PANEL(widget)) {
        // Don't override transparent panels
        // Only style panels with explicit background
    }
    else if (ZUI_IS_TEXTINPUT(widget)) {
        zui_textinput_set_bg_color(ZUI_TEXTINPUT(widget), t->bg_secondary);
        zui_textinput_set_text_color(ZUI_TEXTINPUT(widget), t->text_primary);
        zui_textinput_set_border_color(ZUI_TEXTINPUT(widget), t->border);
        zui_textinput_set_corner_radius(ZUI_TEXTINPUT(widget), t->corner_radius);
    }
    
    // Recurse into children
    int count = zui_widget_get_child_count(widget);
    for (int i = 0; i < count; i++) {
        apply_theme_to_widget(t, zui_widget_get_child(widget, i));
    }
}
```

## Styled Components

### Card Component

```c
ZuiWidget *create_styled_card(Theme *t, const char *title, const char *content)
{
    ZuiWidget *card = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(card), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_padding(ZUI_PANEL(card), t->spacing_md, t->spacing_md, 
                          t->spacing_md, t->spacing_md);
    zui_panel_set_spacing(ZUI_PANEL(card), t->spacing_sm);
    zui_panel_set_background(ZUI_PANEL(card), t->bg_secondary);
    zui_panel_set_corner_radius(ZUI_PANEL(card), t->corner_radius);
    
    ZuiWidget *title_label = zui_label_new(title);
    zui_label_set_size(ZUI_LABEL(title_label), t->font_size_lg);
    zui_label_set_color(ZUI_LABEL(title_label), t->text_primary);
    
    ZuiWidget *content_label = zui_label_new(content);
    zui_label_set_size(ZUI_LABEL(content_label), t->font_size_md);
    zui_label_set_color(ZUI_LABEL(content_label), t->text_secondary);
    
    zui_panel_add_child(ZUI_PANEL(card), title_label);
    zui_panel_add_child(ZUI_PANEL(card), content_label);
    
    return card;
}
```

## Best Practices

1. **Define colors once** — Use constants or theme structs
2. **Be consistent** — Same style for same widget types
3. **Consider contrast** — Ensure text is readable
4. **Test themes** — Try both light and dark
5. **Keep it simple** — Don't over-style, let content shine

---

**See Also:** [Widget Overview](../widgets/overview.md) · [Layout Guide](layout.md)
