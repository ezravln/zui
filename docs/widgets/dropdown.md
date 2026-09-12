# Dropdown

A selection widget that shows a list of options when clicked.

## Basic Usage

```c
ZuiWidget *dropdown = zui_dropdown_new("Select option...");

zui_dropdown_add_item(ZUI_DROPDOWN(dropdown), "Option 1");
zui_dropdown_add_item(ZUI_DROPDOWN(dropdown), "Option 2");
zui_dropdown_add_item(ZUI_DROPDOWN(dropdown), "Option 3");

zui_panel_add_child(ZUI_PANEL(panel), dropdown);
```

## Handling Selection

```c
void on_select(ZuiDropdown *dd, int index, const char *item, void *data)
{
    printf("Selected: %s (index %d)\n", item, index);
}

zui_dropdown_on_change(ZUI_DROPDOWN(dropdown), on_select, NULL);
```

## Getting Selection

```c
// Get selected index (-1 if nothing selected)
int index = zui_dropdown_get_selected_index(ZUI_DROPDOWN(dropdown));

// Get selected item text (NULL if nothing selected)
const char *item = zui_dropdown_get_selected_item(ZUI_DROPDOWN(dropdown));
```

## Managing Items

```c
// Add items
zui_dropdown_add_item(ZUI_DROPDOWN(dd), "Item 1");
zui_dropdown_add_item(ZUI_DROPDOWN(dd), "Item 2");

// Clear all items
zui_dropdown_clear_items(ZUI_DROPDOWN(dd));
```

## Styling

```c
// Colors
zui_dropdown_set_bg_color(ZUI_DROPDOWN(dd), ZUI_COLOR_HEX(0x2c3e50));
zui_dropdown_set_text_color(ZUI_DROPDOWN(dd), ZUI_COLOR_HEX(0xffffff));
zui_dropdown_set_placeholder_color(ZUI_DROPDOWN(dd), ZUI_COLOR_HEX(0x7f8c8d));
zui_dropdown_set_border_color(ZUI_DROPDOWN(dd), ZUI_COLOR_HEX(0x34495e));
zui_dropdown_set_hover_color(ZUI_DROPDOWN(dd), ZUI_COLOR_HEX(0x3498db));
zui_dropdown_set_item_bg_color(ZUI_DROPDOWN(dd), ZUI_COLOR_HEX(0x34495e));

// Appearance
zui_dropdown_set_corner_radius(ZUI_DROPDOWN(dd), 6);
zui_dropdown_set_padding(ZUI_DROPDOWN(dd), 12);
zui_dropdown_set_item_height(ZUI_DROPDOWN(dd), 36);
```

## Examples

### Country Selector

```c
ZuiWidget *create_country_selector(void)
{
    ZuiWidget *dd = zui_dropdown_new("Select country...");
    
    const char *countries[] = {
        "United States", "United Kingdom", "Canada", 
        "Australia", "Germany", "France", "Japan"
    };
    
    for (size_t i = 0; i < sizeof(countries)/sizeof(countries[0]); i++) {
        zui_dropdown_add_item(ZUI_DROPDOWN(dd), countries[i]);
    }
    
    zui_widget_set_size(dd, 200, 40);
    
    return dd;
}
```

### Theme Selector

```c
typedef enum { THEME_LIGHT, THEME_DARK, THEME_SYSTEM } ThemeMode;

void on_theme_change(ZuiDropdown *dd, int index, const char *item, void *data)
{
    ThemeMode *mode = (ThemeMode *)data;
    *mode = (ThemeMode)index;
    
    switch (*mode) {
        case THEME_LIGHT:  apply_light_theme(); break;
        case THEME_DARK:   apply_dark_theme(); break;
        case THEME_SYSTEM: apply_system_theme(); break;
    }
}

ZuiWidget *create_theme_dropdown(ThemeMode *mode)
{
    ZuiWidget *dd = zui_dropdown_new("Theme");
    
    zui_dropdown_add_item(ZUI_DROPDOWN(dd), "Light");
    zui_dropdown_add_item(ZUI_DROPDOWN(dd), "Dark");
    zui_dropdown_add_item(ZUI_DROPDOWN(dd), "System");
    
    zui_dropdown_on_change(ZUI_DROPDOWN(dd), on_theme_change, mode);
    
    return dd;
}
```

### Linked Dropdowns

```c
typedef struct {
    ZuiWidget *country_dd;
    ZuiWidget *city_dd;
} LocationSelector;

const char *us_cities[] = {"New York", "Los Angeles", "Chicago"};
const char *uk_cities[] = {"London", "Manchester", "Birmingham"};

void on_country_change(ZuiDropdown *dd, int index, const char *item, void *data)
{
    LocationSelector *loc = (LocationSelector *)data;
    
    // Clear city dropdown
    zui_dropdown_clear_items(ZUI_DROPDOWN(loc->city_dd));
    
    // Populate based on country
    const char **cities;
    size_t count;
    
    if (index == 0) {  // US
        cities = us_cities;
        count = 3;
    } else {  // UK
        cities = uk_cities;
        count = 3;
    }
    
    for (size_t i = 0; i < count; i++) {
        zui_dropdown_add_item(ZUI_DROPDOWN(loc->city_dd), cities[i]);
    }
}
```

### Form Field

```c
ZuiWidget *create_dropdown_field(const char *label, const char *placeholder,
                                  const char **items, int count)
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 4);
    
    ZuiWidget *lbl = zui_label_new(label);
    zui_label_set_size(ZUI_LABEL(lbl), 14);
    zui_label_set_color(ZUI_LABEL(lbl), ZUI_COLOR_HEX(0xbdc3c7));
    
    ZuiWidget *dd = zui_dropdown_new(placeholder);
    for (int i = 0; i < count; i++) {
        zui_dropdown_add_item(ZUI_DROPDOWN(dd), items[i]);
    }
    zui_widget_set_fill(dd, true, false);
    
    zui_panel_add_child(ZUI_PANEL(container), lbl);
    zui_panel_add_child(ZUI_PANEL(container), dd);
    
    return container;
}
```

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_dropdown_new(placeholder)` | Create dropdown, returns `ZuiWidget *` |
| `zui_dropdown_create(placeholder)` | Create dropdown, returns `ZuiDropdown *` |

### Items

| Function | Description |
|----------|-------------|
| `zui_dropdown_add_item(dd, text)` | Add an item |
| `zui_dropdown_clear_items(dd)` | Remove all items |
| `zui_dropdown_get_selected_index(dd)` | Get selected index (-1 if none) |
| `zui_dropdown_get_selected_item(dd)` | Get selected text (NULL if none) |

### Styling

| Function | Description |
|----------|-------------|
| `zui_dropdown_set_bg_color(dd, color)` | Background color |
| `zui_dropdown_set_text_color(dd, color)` | Text color |
| `zui_dropdown_set_placeholder_color(dd, color)` | Placeholder color |
| `zui_dropdown_set_border_color(dd, color)` | Border color |
| `zui_dropdown_set_hover_color(dd, color)` | Item hover color |
| `zui_dropdown_set_item_bg_color(dd, color)` | Popup background |
| `zui_dropdown_set_corner_radius(dd, radius)` | Corner radius |
| `zui_dropdown_set_padding(dd, padding)` | Inner padding |
| `zui_dropdown_set_item_height(dd, height)` | Item row height |

### Events

| Function | Description |
|----------|-------------|
| `zui_dropdown_on_change(dd, callback, data)` | Selection change handler |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_DROPDOWN(w)` | Check if widget is a dropdown |
| `ZUI_DROPDOWN(w)` | Downcast to `ZuiDropdown *` (or NULL) |

## Notes

- Maximum 64 items per dropdown
- Items are displayed in the order they were added
- The popup closes automatically when an item is selected

---

**See Also:** [TextInput](textinput.md) · [Checkbox](checkbox.md) · [Widget Overview](overview.md)
