# TextInput

A single-line text input field for user text entry.

## Basic Usage

```c
// Create a text input
ZuiWidget *input = zui_textinput_new("Enter your name...");

// Add to parent
zui_panel_add_child(ZUI_PANEL(panel), input);
```

## Handling Changes

```c
void on_text_change(ZuiTextInput *input, const char *text, void *data)
{
    printf("Text changed: %s\n", text);
}

ZuiWidget *input = zui_textinput_new("Search...");
zui_textinput_on_change(ZUI_TEXTINPUT(input), on_text_change, NULL);
```

## Getting and Setting Text

```c
// Set text programmatically
zui_textinput_set_text(ZUI_TEXTINPUT(input), "Hello World");

// Get current text
const char *text = zui_textinput_get_text(ZUI_TEXTINPUT(input));
printf("Current text: %s\n", text);
```

## Styling

### Colors

```c
// Background
zui_textinput_set_bg_color(ZUI_TEXTINPUT(input), ZUI_COLOR_HEX(0x2c3e50));

// Text color
zui_textinput_set_text_color(ZUI_TEXTINPUT(input), ZUI_COLOR_HEX(0xffffff));

// Placeholder color
zui_textinput_set_placeholder_color(ZUI_TEXTINPUT(input), ZUI_COLOR_HEX(0x7f8c8d));

// Border color
zui_textinput_set_border_color(ZUI_TEXTINPUT(input), ZUI_COLOR_HEX(0x3498db));

// Selection highlight
zui_textinput_set_selection_color(ZUI_TEXTINPUT(input), ZUI_COLOR(0.2f, 0.6f, 1.0f, 0.3f));
```

### Corner Radius

```c
zui_textinput_set_corner_radius(ZUI_TEXTINPUT(input), 6);
```

### Padding

```c
zui_textinput_set_padding(ZUI_TEXTINPUT(input), 12);
```

### Size

```c
// Fixed size
zui_widget_set_size(input, 250, 40);

// Fill width
zui_widget_set_fill(input, true, false);
```

## Focus Management

```c
// Focus the input
zui_widget_focus(input);

// Unfocus
zui_widget_unfocus(input);
```

## Examples

### Search Box

```c
ZuiWidget *create_search_box(void (*on_search)(const char *))
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 8);
    
    ZuiWidget *input = zui_textinput_new("Search...");
    zui_widget_set_fill(input, true, false);
    zui_textinput_set_bg_color(ZUI_TEXTINPUT(input), ZUI_COLOR_HEX(0x34495e));
    zui_textinput_set_text_color(ZUI_TEXTINPUT(input), ZUI_COLOR_HEX(0xffffff));
    zui_textinput_set_corner_radius(ZUI_TEXTINPUT(input), 20);
    zui_textinput_set_padding(ZUI_TEXTINPUT(input), 12);
    
    zui_panel_add_child(ZUI_PANEL(container), input);
    
    return container;
}
```

### Login Form

```c
typedef struct {
    ZuiWidget *username;
    ZuiWidget *password;
} LoginForm;

void create_login_form(LoginForm *form, ZuiWidget *parent)
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 12);
    zui_panel_set_size(ZUI_PANEL(container), 300, 0);
    
    // Username
    ZuiWidget *user_label = zui_label_new("Username");
    zui_label_set_color(ZUI_LABEL(user_label), ZUI_COLOR_HEX(0xffffff));
    
    form->username = zui_textinput_new("Enter username");
    zui_widget_set_fill(form->username, true, false);
    
    // Password (note: ZUI doesn't have password masking yet)
    ZuiWidget *pass_label = zui_label_new("Password");
    zui_label_set_color(ZUI_LABEL(pass_label), ZUI_COLOR_HEX(0xffffff));
    
    form->password = zui_textinput_new("Enter password");
    zui_widget_set_fill(form->password, true, false);
    
    // Submit button
    ZuiWidget *submit = zui_button_new("Login");
    zui_widget_set_fill(submit, true, false);
    
    zui_panel_add_child(ZUI_PANEL(container), user_label);
    zui_panel_add_child(ZUI_PANEL(container), form->username);
    zui_panel_add_child(ZUI_PANEL(container), pass_label);
    zui_panel_add_child(ZUI_PANEL(container), form->password);
    zui_panel_add_child(ZUI_PANEL(container), submit);
    
    zui_panel_add_child(ZUI_PANEL(parent), container);
}
```

### Validated Input

```c
typedef struct {
    ZuiWidget *input;
    ZuiWidget *error_label;
    bool is_valid;
} ValidatedInput;

void validate_email(ZuiTextInput *input, const char *text, void *data)
{
    ValidatedInput *vi = (ValidatedInput *)data;
    
    // Simple email validation
    vi->is_valid = (strchr(text, '@') != NULL);
    
    if (vi->is_valid) {
        zui_widget_set_visible(vi->error_label, false);
        zui_textinput_set_border_color(input, ZUI_COLOR_HEX(0x2ecc71));
    } else {
        zui_widget_set_visible(vi->error_label, true);
        zui_textinput_set_border_color(input, ZUI_COLOR_HEX(0xe74c3c));
    }
}

ZuiWidget *create_email_input(ValidatedInput *vi)
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 4);
    
    vi->input = zui_textinput_new("Email address");
    zui_textinput_on_change(ZUI_TEXTINPUT(vi->input), validate_email, vi);
    
    vi->error_label = zui_label_new("Please enter a valid email");
    zui_label_set_size(ZUI_LABEL(vi->error_label), 12);
    zui_label_set_color(ZUI_LABEL(vi->error_label), ZUI_COLOR_HEX(0xe74c3c));
    zui_widget_set_visible(vi->error_label, false);
    
    zui_panel_add_child(ZUI_PANEL(container), vi->input);
    zui_panel_add_child(ZUI_PANEL(container), vi->error_label);
    
    return container;
}
```

## Keyboard Handling

TextInput handles standard keyboard operations:

| Key | Action |
|-----|--------|
| Left/Right | Move cursor |
| Home/End | Jump to start/end |
| Backspace | Delete character before cursor |
| Delete | Delete character after cursor |
| Ctrl+A | Select all |
| Ctrl+C | Copy selection |
| Ctrl+V | Paste |
| Ctrl+X | Cut selection |

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_textinput_new(placeholder)` | Create input, returns `ZuiWidget *` |
| `zui_textinput_create(placeholder)` | Create input, returns `ZuiTextInput *` |

### Text

| Function | Description |
|----------|-------------|
| `zui_textinput_set_text(input, text)` | Set text content |
| `zui_textinput_get_text(input)` | Get text content |
| `zui_textinput_set_placeholder(input, text)` | Set placeholder text |

### Styling

| Function | Description |
|----------|-------------|
| `zui_textinput_set_bg_color(input, color)` | Background color |
| `zui_textinput_set_text_color(input, color)` | Text color |
| `zui_textinput_set_placeholder_color(input, color)` | Placeholder color |
| `zui_textinput_set_border_color(input, color)` | Border color |
| `zui_textinput_set_selection_color(input, color)` | Selection highlight |
| `zui_textinput_set_corner_radius(input, radius)` | Corner radius |
| `zui_textinput_set_padding(input, padding)` | Inner padding |

### Events

| Function | Description |
|----------|-------------|
| `zui_textinput_on_change(input, callback, data)` | Text change handler |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_TEXTINPUT(w)` | Check if widget is a text input |
| `ZUI_TEXTINPUT(w)` | Downcast to `ZuiTextInput *` (or NULL) |

## Notes

- Maximum text length is 1024 characters by default
- For multi-line text, use [TextArea](textarea.md)
- Password masking is not yet implemented

---

**See Also:** [TextArea](textarea.md) · [Dropdown](dropdown.md) · [Widget Overview](overview.md)
