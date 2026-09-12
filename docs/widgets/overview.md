# Widget System Overview

Widgets are the building blocks of ZUI applications. Every visual element — buttons, labels, panels, inputs — is a widget.

## The Unified Widget Model

ZUI uses a GTK-like unified widget model where all widgets share a common base type:

```c
// All creation functions return ZuiWidget *
ZuiWidget *button = zui_button_new("Click");
ZuiWidget *label = zui_label_new("Hello");
ZuiWidget *panel = zui_panel_new();
```

### Type Checking

Check a widget's type at runtime:

```c
if (ZUI_IS_BUTTON(widget)) {
    printf("It's a button!\n");
}

if (ZUI_IS_LABEL(widget)) {
    printf("It's a label!\n");
}
```

### Safe Downcasting

Convert to specific widget types safely:

```c
// Returns NULL if widget is not a button
ZuiButton *button = ZUI_BUTTON(widget);
if (button) {
    zui_button_set_text(button, "New Text");
}

// Common pattern: check and use in one expression
if (ZUI_IS_LABEL(widget)) {
    zui_label_set_color(ZUI_LABEL(widget), ZUI_COLOR_HEX(0xff0000));
}
```

### Generic Operations

Some operations work on any widget:

```c
// Visibility
zui_widget_set_visible(widget, false);
bool visible = zui_widget_is_visible(widget);

// Styling
zui_widget_set_background(widget, ZUI_COLOR_HEX(0x333333));
zui_widget_set_corner_radius(widget, 8);

// Size
zui_widget_set_size(widget, 200, 50);
zui_widget_set_fill(widget, true, false);

// Hierarchy
zui_widget_add_child(parent, child);
zui_widget_remove_child(parent, child);
ZuiWidget *parent = zui_widget_get_parent(widget);
int count = zui_widget_get_child_count(widget);

// Type info
ZuiWidgetType type = zui_widget_get_type(widget);
```

## Widget Hierarchy

Widgets form a tree structure:

```
Window
└── Content
    └── Panel
        ├── Label ("Title")
        ├── TextInput
        └── Panel
            ├── Button ("Cancel")
            └── Button ("OK")
```

### Adding Children

```c
ZuiWidget *panel = zui_panel_new();
ZuiWidget *label = zui_label_new("Hello");
ZuiWidget *button = zui_button_new("Click");

// For panels, use panel-specific function
zui_panel_add_child(ZUI_PANEL(panel), label);
zui_panel_add_child(ZUI_PANEL(panel), button);

// Generic function also works
zui_widget_add_child(panel, label);
```

### Removing Children

```c
zui_widget_remove_child(parent, child);

// Clear all children from a panel
zui_panel_clear(ZUI_PANEL(panel));
```

## Widget Catalog

### Basic Widgets

| Widget | Description | Creation |
|--------|-------------|----------|
| [Button](button.md) | Clickable button | `zui_button_new(text)` |
| [Label](label.md) | Text display | `zui_label_new(text)` |
| [Icon](icon.md) | SVG icon display | `zui_icon_new(path)` |
| [Image](image.md) | Image display | `zui_image_new(path)` |

### Input Widgets

| Widget | Description | Creation |
|--------|-------------|----------|
| [TextInput](textinput.md) | Single-line text input | `zui_textinput_new(placeholder)` |
| [TextArea](textarea.md) | Multi-line text editor | `zui_textarea_new(placeholder)` |
| [Checkbox](checkbox.md) | Boolean toggle | `zui_checkbox_new(label)` |
| [RadioButton](radiobutton.md) | Single selection | `zui_radiobutton_create(label, group)` |
| [Slider](slider.md) | Value slider | `zui_slider_new(min, max, value)` |
| [Dropdown](dropdown.md) | Selection dropdown | `zui_dropdown_new(placeholder)` |

### Container Widgets

| Widget | Description | Creation |
|--------|-------------|----------|
| [Panel](panel.md) | Layout container | `zui_panel_new()` |
| [ScrollView](scrollview.md) | Scrollable container | `zui_scrollview_new()` |
| [SplitView](splitview.md) | Resizable split panes | `zui_splitview_new(vertical)` |
| [TabView](tabview.md) | Tabbed container | `zui_tabview_create()` |
| [GridView](gridview.md) | Grid layout | `zui_gridview_new(columns)` |

### Display Widgets

| Widget | Description | Creation |
|--------|-------------|----------|
| [ProgressBar](progressbar.md) | Linear progress | `zui_progressbar_new()` |
| [CircularProgress](circularprogress.md) | Circular progress | `zui_circularprogress_new()` |
| [PieChart](charts.md) | Pie chart | `zui_piechart_new()` |
| [BarChart](charts.md) | Bar chart | `zui_barchart_new()` |
| [LineChart](charts.md) | Line chart | `zui_linechart_new()` |

### Navigation Widgets

| Widget | Description | Creation |
|--------|-------------|----------|
| [MenuBar](menubar.md) | Application menu bar | `zui_menubar_create()` |
| [Menu](menu.md) | Dropdown menu | `zui_menu_create(title)` |
| [TreeView](treeview.md) | Hierarchical tree | `zui_treeview_create()` |

### Media Widgets

| Widget | Description | Creation |
|--------|-------------|----------|
| [Video](video.md) | Video playback | `zui_video_create(path)` |

## Widget Lifecycle

### Creation

```c
// Using _new() functions (returns ZuiWidget *)
ZuiWidget *button = zui_button_new("Click");

// Using _create() functions (returns specific type)
ZuiButton *button = zui_button_create("Click");
```

### Configuration

```c
// Configure after creation
zui_button_set_colors(ZUI_BUTTON(button),
    ZUI_COLOR_HEX(0x3498db),  // normal
    ZUI_COLOR_HEX(0x5dade2),  // hover
    ZUI_COLOR_HEX(0x2980b9)); // pressed
zui_button_set_corner_radius(ZUI_BUTTON(button), 6);
```

### Adding to Tree

```c
// Add to parent
zui_panel_add_child(ZUI_PANEL(panel), button);

// Or add to window content
zui_widget_add_child(zui_window_content(window), button);
```

### Destruction

Widgets are automatically destroyed when their parent is destroyed:

```c
// Destroying window destroys all children
zui_window_destroy(window);

// Or manually remove and destroy
zui_widget_remove_child(parent, widget);
zui_widget_destroy(widget);
```

## Common Patterns

### Factory Functions

Create styled widgets consistently:

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
    return btn;
}
```

### Widget References

Store references for later updates:

```c
typedef struct {
    ZuiWidget *status_label;
    ZuiWidget *progress_bar;
} AppWidgets;

static AppWidgets widgets;

void create_ui(void)
{
    widgets.status_label = zui_label_new("Ready");
    widgets.progress_bar = zui_progressbar_new();
}

void update_progress(float value)
{
    zui_progressbar_set_value(ZUI_PROGRESSBAR(widgets.progress_bar), value);
    
    char buf[32];
    snprintf(buf, sizeof(buf), "Progress: %.0f%%", value * 100);
    zui_label_set_text(ZUI_LABEL(widgets.status_label), buf);
}
```

### Dynamic Widget Creation

Create widgets at runtime:

```c
void add_list_item(ZuiPanel *list, const char *text)
{
    ZuiWidget *item = zui_label_new(text);
    zui_label_set_color(ZUI_LABEL(item), ZUI_COLOR_HEX(0xffffff));
    zui_panel_add_child(list, item);
}

// Usage
for (int i = 0; i < item_count; i++) {
    add_list_item(ZUI_PANEL(list), items[i]);
}
```

## Type Macros Reference

### Type Checking Macros

| Macro | Widget Type |
|-------|-------------|
| `ZUI_IS_BUTTON(w)` | ZuiButton |
| `ZUI_IS_LABEL(w)` | ZuiLabel |
| `ZUI_IS_PANEL(w)` | ZuiPanel |
| `ZUI_IS_CHECKBOX(w)` | ZuiCheckbox |
| `ZUI_IS_TEXTINPUT(w)` | ZuiTextInput |
| `ZUI_IS_SLIDER(w)` | ZuiSlider |
| `ZUI_IS_DROPDOWN(w)` | ZuiDropdown |
| `ZUI_IS_SCROLLVIEW(w)` | ZuiScrollView |
| `ZUI_IS_PROGRESSBAR(w)` | ZuiProgressBar |
| `ZUI_IS_CIRCULAR_PROGRESS(w)` | ZuiCircularProgress |
| `ZUI_IS_GRIDVIEW(w)` | ZuiGridView |
| `ZUI_IS_PIECHART(w)` | ZuiPieChart |
| `ZUI_IS_BARCHART(w)` | ZuiBarChart |
| `ZUI_IS_LINECHART(w)` | ZuiLineChart |
| `ZUI_IS_IMAGE(w)` | ZuiImage |
| `ZUI_IS_ICON(w)` | ZuiIcon |

### Downcast Macros

Each `ZUI_IS_*` macro has a corresponding `ZUI_*` downcast macro:

```c
ZUI_BUTTON(w)      // Returns ZuiButton* or NULL
ZUI_LABEL(w)       // Returns ZuiLabel* or NULL
ZUI_PANEL(w)       // Returns ZuiPanel* or NULL
// ... etc
```

---

**Next:** [Button →](button.md)
