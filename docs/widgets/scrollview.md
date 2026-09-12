# ScrollView

A container that enables scrolling when content exceeds visible bounds.

## Basic Usage

```c
ZuiWidget *scroll = zui_scrollview_new();

// Create content that may exceed scroll view size
ZuiWidget *content = zui_panel_new();
zui_panel_set_layout(ZUI_PANEL(content), ZUI_LAYOUT_VERTICAL);
// ... add many children to content ...

zui_scrollview_set_content(ZUI_SCROLLVIEW(scroll), content);
zui_panel_add_child(ZUI_PANEL(parent), scroll);
```

## Scroll Control

```c
// Scroll to position
zui_scrollview_scroll_to(ZUI_SCROLLVIEW(sv), 0, 100);

// Get current scroll position
float x, y;
zui_scrollview_get_scroll(ZUI_SCROLLVIEW(sv), &x, &y);
```

## Sizing

```c
// Fixed size
zui_widget_set_size(scroll, 400, 300);

// Fill available space
zui_widget_set_fill(scroll, true, true);
```

## Examples

### Long List

```c
ZuiWidget *create_scrollable_list(const char **items, int count)
{
    ZuiWidget *scroll = zui_scrollview_new();
    zui_widget_set_fill(scroll, true, true);
    
    ZuiWidget *content = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(content), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(content), 2);
    zui_panel_set_fill(ZUI_PANEL(content), true, false);
    
    for (int i = 0; i < count; i++) {
        ZuiWidget *item = zui_panel_new();
        zui_panel_set_layout(ZUI_PANEL(item), ZUI_LAYOUT_HORIZONTAL);
        zui_panel_set_padding(ZUI_PANEL(item), 12, 16, 12, 16);
        zui_panel_set_fill(ZUI_PANEL(item), true, false);
        zui_panel_set_background(ZUI_PANEL(item), ZUI_COLOR_HEX(0x2c3e50));
        
        ZuiWidget *label = zui_label_new(items[i]);
        zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR_HEX(0xffffff));
        
        zui_panel_add_child(ZUI_PANEL(item), label);
        zui_panel_add_child(ZUI_PANEL(content), item);
    }
    
    zui_scrollview_set_content(ZUI_SCROLLVIEW(scroll), content);
    
    return scroll;
}
```

### Message Feed

```c
typedef struct {
    const char *author;
    const char *text;
    const char *time;
} Message;

ZuiWidget *create_message_widget(Message *msg)
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 4);
    zui_panel_set_padding(ZUI_PANEL(container), 12, 16, 12, 16);
    zui_panel_set_background(ZUI_PANEL(container), ZUI_COLOR_HEX(0x34495e));
    zui_panel_set_corner_radius(ZUI_PANEL(container), 8);
    zui_panel_set_fill(ZUI_PANEL(container), true, false);
    
    // Header: author + time
    ZuiWidget *header = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(header), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_fill(ZUI_PANEL(header), true, false);
    
    ZuiWidget *author = zui_label_new(msg->author);
    zui_label_set_size(ZUI_LABEL(author), 14);
    zui_label_set_color(ZUI_LABEL(author), ZUI_COLOR_HEX(0x3498db));
    
    ZuiWidget *time_lbl = zui_label_new(msg->time);
    zui_label_set_size(ZUI_LABEL(time_lbl), 12);
    zui_label_set_color(ZUI_LABEL(time_lbl), ZUI_COLOR_HEX(0x7f8c8d));
    
    zui_panel_add_child(ZUI_PANEL(header), author);
    zui_panel_add_child(ZUI_PANEL(header), time_lbl);
    
    // Message text
    ZuiWidget *text = zui_label_new(msg->text);
    zui_label_set_color(ZUI_LABEL(text), ZUI_COLOR_HEX(0xffffff));
    
    zui_panel_add_child(ZUI_PANEL(container), header);
    zui_panel_add_child(ZUI_PANEL(container), text);
    
    return container;
}

ZuiWidget *create_message_feed(Message *messages, int count)
{
    ZuiWidget *scroll = zui_scrollview_new();
    zui_widget_set_fill(scroll, true, true);
    
    ZuiWidget *content = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(content), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(content), 8);
    zui_panel_set_padding(ZUI_PANEL(content), 8, 8, 8, 8);
    zui_panel_set_fill(ZUI_PANEL(content), true, false);
    
    for (int i = 0; i < count; i++) {
        zui_panel_add_child(ZUI_PANEL(content), create_message_widget(&messages[i]));
    }
    
    zui_scrollview_set_content(ZUI_SCROLLVIEW(scroll), content);
    
    // Scroll to bottom (most recent)
    // Note: do this after layout pass
    
    return scroll;
}
```

### Card Grid

```c
ZuiWidget *create_card_grid(int card_count)
{
    ZuiWidget *scroll = zui_scrollview_new();
    zui_widget_set_fill(scroll, true, true);
    
    ZuiWidget *grid = zui_gridview_new(3);
    zui_gridview_set_gap(ZUI_GRIDVIEW(grid), 16, 16);
    zui_gridview_set_padding(ZUI_GRIDVIEW(grid), 16);
    
    for (int i = 0; i < card_count; i++) {
        ZuiWidget *card = zui_panel_new();
        zui_panel_set_layout(ZUI_PANEL(card), ZUI_LAYOUT_VERTICAL);
        zui_panel_set_padding(ZUI_PANEL(card), 16, 16, 16, 16);
        zui_panel_set_background(ZUI_PANEL(card), ZUI_COLOR_HEX(0x34495e));
        zui_panel_set_corner_radius(ZUI_PANEL(card), 8);
        zui_widget_set_size(card, 0, 150);
        
        char title[32];
        snprintf(title, sizeof(title), "Card %d", i + 1);
        
        ZuiWidget *label = zui_label_new(title);
        zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR_HEX(0xffffff));
        
        zui_panel_add_child(ZUI_PANEL(card), label);
        zui_gridview_add_child(ZUI_GRIDVIEW(grid), card);
    }
    
    zui_scrollview_set_content(ZUI_SCROLLVIEW(scroll), (ZuiWidget *)grid);
    
    return scroll;
}
```

### Settings Page

```c
ZuiWidget *create_settings_page(void)
{
    ZuiWidget *scroll = zui_scrollview_new();
    zui_widget_set_fill(scroll, true, true);
    
    ZuiWidget *content = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(content), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(content), 24);
    zui_panel_set_padding(ZUI_PANEL(content), 24, 24, 24, 24);
    zui_panel_set_fill(ZUI_PANEL(content), true, false);
    
    // Section: Account
    ZuiWidget *account_section = create_settings_section("Account",
        (const char *[]){"Profile", "Email", "Password", "Two-factor"}, 4);
    
    // Section: Appearance
    ZuiWidget *appearance_section = create_settings_section("Appearance",
        (const char *[]){"Theme", "Font size", "Animations", "Compact mode"}, 4);
    
    // Section: Privacy
    ZuiWidget *privacy_section = create_settings_section("Privacy",
        (const char *[]){"Activity status", "Read receipts", "Typing indicator"}, 3);
    
    // Section: Notifications
    ZuiWidget *notif_section = create_settings_section("Notifications",
        (const char *[]){"Push", "Email", "Sounds", "Desktop"}, 4);
    
    zui_panel_add_child(ZUI_PANEL(content), account_section);
    zui_panel_add_child(ZUI_PANEL(content), appearance_section);
    zui_panel_add_child(ZUI_PANEL(content), privacy_section);
    zui_panel_add_child(ZUI_PANEL(content), notif_section);
    
    zui_scrollview_set_content(ZUI_SCROLLVIEW(scroll), content);
    
    return scroll;
}

ZuiWidget *create_settings_section(const char *title, const char **items, int count)
{
    ZuiWidget *section = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(section), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(section), 8);
    zui_panel_set_fill(ZUI_PANEL(section), true, false);
    
    ZuiWidget *header = zui_label_new(title);
    zui_label_set_size(ZUI_LABEL(header), 18);
    zui_label_set_color(ZUI_LABEL(header), ZUI_COLOR_HEX(0xffffff));
    
    zui_panel_add_child(ZUI_PANEL(section), header);
    
    for (int i = 0; i < count; i++) {
        ZuiWidget *row = zui_panel_new();
        zui_panel_set_layout(ZUI_PANEL(row), ZUI_LAYOUT_HORIZONTAL);
        zui_panel_set_padding(ZUI_PANEL(row), 12, 16, 12, 16);
        zui_panel_set_background(ZUI_PANEL(row), ZUI_COLOR_HEX(0x2c3e50));
        zui_panel_set_fill(ZUI_PANEL(row), true, false);
        
        if (i == 0) {
            zui_panel_set_corner_radius(ZUI_PANEL(row), 8);  // Top corners
        } else if (i == count - 1) {
            zui_panel_set_corner_radius(ZUI_PANEL(row), 8);  // Bottom corners
        }
        
        ZuiWidget *label = zui_label_new(items[i]);
        zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR_HEX(0xbdc3c7));
        
        zui_panel_add_child(ZUI_PANEL(row), label);
        zui_panel_add_child(ZUI_PANEL(section), row);
    }
    
    return section;
}
```

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_scrollview_new()` | Create scroll view, returns `ZuiWidget *` |
| `zui_scrollview_create()` | Create scroll view, returns `ZuiScrollView *` |

### Content

| Function | Description |
|----------|-------------|
| `zui_scrollview_set_content(sv, widget)` | Set scrollable content |

### Scrolling

| Function | Description |
|----------|-------------|
| `zui_scrollview_scroll_to(sv, x, y)` | Scroll to position |
| `zui_scrollview_get_scroll(sv, x, y)` | Get current scroll position |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_SCROLLVIEW(w)` | Check if widget is a scroll view |
| `ZUI_SCROLLVIEW(w)` | Downcast to `ZuiScrollView *` (or NULL) |

## Notes

- Content widget is owned by the scroll view
- Scroll position resets when content changes
- Mouse wheel and touch gestures work automatically

---

**See Also:** [Panel](panel.md) · [GridView](gridview.md) · [Widget Overview](overview.md)
