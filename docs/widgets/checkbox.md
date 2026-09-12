# Checkbox

A toggle widget for boolean values.

## Basic Usage

```c
ZuiWidget *checkbox = zui_checkbox_new("Enable notifications");
zui_panel_add_child(ZUI_PANEL(panel), checkbox);
```

## Handling Changes

```c
void on_change(ZuiCheckbox *cb, bool checked, void *data)
{
    printf("Checked: %s\n", checked ? "yes" : "no");
}

ZuiWidget *checkbox = zui_checkbox_new("Accept terms");
zui_checkbox_on_change(ZUI_CHECKBOX(checkbox), on_change, NULL);
```

## Getting/Setting State

```c
// Set programmatically
zui_checkbox_set_checked(ZUI_CHECKBOX(checkbox), true);

// Get current state
bool is_checked = zui_checkbox_is_checked(ZUI_CHECKBOX(checkbox));
```

## Styling

```c
// Box colors
zui_checkbox_set_box_color(ZUI_CHECKBOX(cb), ZUI_COLOR_HEX(0x34495e));
zui_checkbox_set_checked_box_color(ZUI_CHECKBOX(cb), ZUI_COLOR_HEX(0x3498db));

// Check icon color
zui_checkbox_set_icon_color(ZUI_CHECKBOX(cb), ZUI_COLOR_HEX(0xffffff));

// Label color
zui_checkbox_set_label_color(ZUI_CHECKBOX(cb), ZUI_COLOR_HEX(0xffffff));

// Box size
zui_checkbox_set_box_size(ZUI_CHECKBOX(cb), 20);

// Corner radius
zui_checkbox_set_corner_radius(ZUI_CHECKBOX(cb), 4);
```

## Examples

### Settings Panel

```c
typedef struct {
    bool notifications;
    bool dark_mode;
    bool auto_save;
} Settings;

void on_setting_change(ZuiCheckbox *cb, bool checked, void *data)
{
    Settings *s = (Settings *)data;
    const char *label = zui_checkbox_get_label(cb);
    
    if (strstr(label, "Notifications")) {
        s->notifications = checked;
    } else if (strstr(label, "Dark")) {
        s->dark_mode = checked;
    } else if (strstr(label, "Auto")) {
        s->auto_save = checked;
    }
}

ZuiWidget *create_settings_panel(Settings *settings)
{
    ZuiWidget *panel = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(panel), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(panel), 12);
    
    ZuiWidget *cb1 = zui_checkbox_new("Enable Notifications");
    ZuiWidget *cb2 = zui_checkbox_new("Dark Mode");
    ZuiWidget *cb3 = zui_checkbox_new("Auto-save");
    
    zui_checkbox_set_checked(ZUI_CHECKBOX(cb1), settings->notifications);
    zui_checkbox_set_checked(ZUI_CHECKBOX(cb2), settings->dark_mode);
    zui_checkbox_set_checked(ZUI_CHECKBOX(cb3), settings->auto_save);
    
    zui_checkbox_on_change(ZUI_CHECKBOX(cb1), on_setting_change, settings);
    zui_checkbox_on_change(ZUI_CHECKBOX(cb2), on_setting_change, settings);
    zui_checkbox_on_change(ZUI_CHECKBOX(cb3), on_setting_change, settings);
    
    zui_panel_add_child(ZUI_PANEL(panel), cb1);
    zui_panel_add_child(ZUI_PANEL(panel), cb2);
    zui_panel_add_child(ZUI_PANEL(panel), cb3);
    
    return panel;
}
```

### Terms Agreement

```c
typedef struct {
    ZuiWidget *checkbox;
    ZuiWidget *submit_button;
} TermsForm;

void on_terms_change(ZuiCheckbox *cb, bool checked, void *data)
{
    TermsForm *form = (TermsForm *)data;
    
    if (checked) {
        zui_button_set_colors(ZUI_BUTTON(form->submit_button),
            ZUI_COLOR_HEX(0x3498db),
            ZUI_COLOR_HEX(0x5dade2),
            ZUI_COLOR_HEX(0x2980b9));
    } else {
        zui_button_set_colors(ZUI_BUTTON(form->submit_button),
            ZUI_COLOR_HEX(0x7f8c8d),
            ZUI_COLOR_HEX(0x95a5a6),
            ZUI_COLOR_HEX(0x6c7a7d));
    }
}
```

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_checkbox_new(label)` | Create checkbox, returns `ZuiWidget *` |
| `zui_checkbox_create(label)` | Create checkbox, returns `ZuiCheckbox *` |

### State

| Function | Description |
|----------|-------------|
| `zui_checkbox_set_checked(cb, checked)` | Set checked state |
| `zui_checkbox_is_checked(cb)` | Get checked state |
| `zui_checkbox_get_label(cb)` | Get label text |

### Styling

| Function | Description |
|----------|-------------|
| `zui_checkbox_set_box_color(cb, color)` | Unchecked box color |
| `zui_checkbox_set_checked_box_color(cb, color)` | Checked box color |
| `zui_checkbox_set_icon_color(cb, color)` | Check mark color |
| `zui_checkbox_set_label_color(cb, color)` | Label text color |
| `zui_checkbox_set_box_size(cb, size)` | Box size in pixels |
| `zui_checkbox_set_corner_radius(cb, radius)` | Box corner radius |

### Events

| Function | Description |
|----------|-------------|
| `zui_checkbox_on_change(cb, callback, data)` | State change handler |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_CHECKBOX(w)` | Check if widget is a checkbox |
| `ZUI_CHECKBOX(w)` | Downcast to `ZuiCheckbox *` (or NULL) |

---

**See Also:** [RadioButton](radiobutton.md) · [Button](button.md) · [Widget Overview](overview.md)
