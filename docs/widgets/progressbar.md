# ProgressBar

A horizontal bar that displays completion progress.

## Basic Usage

```c
ZuiWidget *progress = zui_progressbar_new();
zui_progressbar_set_value(ZUI_PROGRESSBAR(progress), 0.5f);  // 50%

zui_panel_add_child(ZUI_PANEL(panel), progress);
```

## Setting Progress

Progress value is between 0.0 (empty) and 1.0 (full):

```c
// Empty
zui_progressbar_set_value(ZUI_PROGRESSBAR(bar), 0.0f);

// Half
zui_progressbar_set_value(ZUI_PROGRESSBAR(bar), 0.5f);

// Full
zui_progressbar_set_value(ZUI_PROGRESSBAR(bar), 1.0f);

// Get current value
float value = zui_progressbar_get_value(ZUI_PROGRESSBAR(bar));
```

## Styling

```c
// Colors
zui_progressbar_set_colors(ZUI_PROGRESSBAR(bar),
    ZUI_COLOR_HEX(0x34495e),   // Track (background)
    ZUI_COLOR_HEX(0x3498db));  // Fill (progress)

// Rounded corners
zui_progressbar_set_corner_radius(ZUI_PROGRESSBAR(bar), 4);

// Size
zui_widget_set_size(bar, 200, 8);  // Width x Height
```

## Examples

### File Download

```c
typedef struct {
    ZuiWidget *progress_bar;
    ZuiWidget *status_label;
    float bytes_received;
    float total_bytes;
} DownloadState;

void update_download_progress(DownloadState *state, float bytes)
{
    state->bytes_received += bytes;
    float progress = state->bytes_received / state->total_bytes;
    
    zui_progressbar_set_value(ZUI_PROGRESSBAR(state->progress_bar), progress);
    
    char buf[64];
    snprintf(buf, sizeof(buf), "%.1f MB / %.1f MB (%.0f%%)",
        state->bytes_received / 1e6,
        state->total_bytes / 1e6,
        progress * 100);
    zui_label_set_text(ZUI_LABEL(state->status_label), buf);
}

ZuiWidget *create_download_ui(DownloadState *state)
{
    ZuiWidget *panel = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(panel), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(panel), 8);
    
    state->progress_bar = zui_progressbar_new();
    zui_widget_set_size(state->progress_bar, 300, 12);
    zui_progressbar_set_colors(ZUI_PROGRESSBAR(state->progress_bar),
        ZUI_COLOR_HEX(0x2c3e50),
        ZUI_COLOR_HEX(0x2ecc71));
    zui_progressbar_set_corner_radius(ZUI_PROGRESSBAR(state->progress_bar), 6);
    
    state->status_label = zui_label_new("Starting download...");
    zui_label_set_size(ZUI_LABEL(state->status_label), 14);
    zui_label_set_color(ZUI_LABEL(state->status_label), ZUI_COLOR_HEX(0x95a5a6));
    
    zui_panel_add_child(ZUI_PANEL(panel), state->progress_bar);
    zui_panel_add_child(ZUI_PANEL(panel), state->status_label);
    
    return panel;
}
```

### Multi-Step Process

```c
typedef struct {
    ZuiWidget *bars[4];
    ZuiWidget *labels[4];
    int current_step;
} ProcessState;

const char *step_names[] = {
    "Downloading", "Extracting", "Installing", "Configuring"
};

ZuiWidget *create_process_ui(ProcessState *state)
{
    ZuiWidget *panel = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(panel), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(panel), 16);
    
    for (int i = 0; i < 4; i++) {
        ZuiWidget *row = zui_panel_new();
        zui_panel_set_layout(ZUI_PANEL(row), ZUI_LAYOUT_VERTICAL);
        zui_panel_set_spacing(ZUI_PANEL(row), 4);
        
        state->labels[i] = zui_label_new(step_names[i]);
        zui_label_set_size(ZUI_LABEL(state->labels[i]), 14);
        zui_label_set_color(ZUI_LABEL(state->labels[i]), ZUI_COLOR_HEX(0x7f8c8d));
        
        state->bars[i] = zui_progressbar_new();
        zui_widget_set_size(state->bars[i], 250, 8);
        zui_progressbar_set_colors(ZUI_PROGRESSBAR(state->bars[i]),
            ZUI_COLOR_HEX(0x2c3e50),
            ZUI_COLOR_HEX(0x7f8c8d));  // Inactive color
        
        zui_panel_add_child(ZUI_PANEL(row), state->labels[i]);
        zui_panel_add_child(ZUI_PANEL(row), state->bars[i]);
        zui_panel_add_child(ZUI_PANEL(panel), row);
    }
    
    return panel;
}

void set_active_step(ProcessState *state, int step, float progress)
{
    // Mark previous steps as complete
    for (int i = 0; i < step; i++) {
        zui_progressbar_set_value(ZUI_PROGRESSBAR(state->bars[i]), 1.0f);
        zui_progressbar_set_colors(ZUI_PROGRESSBAR(state->bars[i]),
            ZUI_COLOR_HEX(0x2c3e50),
            ZUI_COLOR_HEX(0x2ecc71));
        zui_label_set_color(ZUI_LABEL(state->labels[i]), ZUI_COLOR_HEX(0x2ecc71));
    }
    
    // Update current step
    zui_progressbar_set_value(ZUI_PROGRESSBAR(state->bars[step]), progress);
    zui_progressbar_set_colors(ZUI_PROGRESSBAR(state->bars[step]),
        ZUI_COLOR_HEX(0x2c3e50),
        ZUI_COLOR_HEX(0x3498db));
    zui_label_set_color(ZUI_LABEL(state->labels[step]), ZUI_COLOR_HEX(0xffffff));
}
```

### Storage Usage

```c
ZuiWidget *create_storage_indicator(float used_gb, float total_gb)
{
    ZuiWidget *panel = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(panel), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(panel), 8);
    
    // Title row
    ZuiWidget *header = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(header), ZUI_LAYOUT_HORIZONTAL);
    
    ZuiWidget *title = zui_label_new("Storage");
    zui_label_set_color(ZUI_LABEL(title), ZUI_COLOR_HEX(0xffffff));
    
    char usage_text[32];
    snprintf(usage_text, sizeof(usage_text), "%.1f GB / %.0f GB", used_gb, total_gb);
    ZuiWidget *usage = zui_label_new(usage_text);
    zui_label_set_size(ZUI_LABEL(usage), 14);
    zui_label_set_color(ZUI_LABEL(usage), ZUI_COLOR_HEX(0x95a5a6));
    
    zui_panel_add_child(ZUI_PANEL(header), title);
    zui_panel_add_child(ZUI_PANEL(header), usage);
    
    // Progress bar
    float ratio = used_gb / total_gb;
    ZuiWidget *bar = zui_progressbar_new();
    zui_widget_set_size(bar, 300, 16);
    zui_widget_set_fill(bar, true, false);
    zui_progressbar_set_value(ZUI_PROGRESSBAR(bar), ratio);
    zui_progressbar_set_corner_radius(ZUI_PROGRESSBAR(bar), 8);
    
    // Color based on usage
    ZuiColor fill_color;
    if (ratio > 0.9f) {
        fill_color = ZUI_COLOR_HEX(0xe74c3c);  // Red - critical
    } else if (ratio > 0.75f) {
        fill_color = ZUI_COLOR_HEX(0xf1c40f);  // Yellow - warning
    } else {
        fill_color = ZUI_COLOR_HEX(0x3498db);  // Blue - normal
    }
    
    zui_progressbar_set_colors(ZUI_PROGRESSBAR(bar),
        ZUI_COLOR_HEX(0x2c3e50), fill_color);
    
    zui_panel_add_child(ZUI_PANEL(panel), header);
    zui_panel_add_child(ZUI_PANEL(panel), bar);
    
    return panel;
}
```

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_progressbar_new()` | Create progress bar, returns `ZuiWidget *` |
| `zui_progressbar_create()` | Create progress bar, returns `ZuiProgressBar *` |

### Value

| Function | Description |
|----------|-------------|
| `zui_progressbar_set_value(bar, value)` | Set progress (0.0 - 1.0) |
| `zui_progressbar_get_value(bar)` | Get current progress |

### Styling

| Function | Description |
|----------|-------------|
| `zui_progressbar_set_colors(bar, track, fill)` | Set track and fill colors |
| `zui_progressbar_set_corner_radius(bar, radius)` | Set corner radius |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_PROGRESSBAR(w)` | Check if widget is a progress bar |
| `ZUI_PROGRESSBAR(w)` | Downcast to `ZuiProgressBar *` (or NULL) |

---

**See Also:** [CircularProgress](circularprogress.md) · [Slider](slider.md) · [Widget Overview](overview.md)
