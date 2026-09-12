# CircularProgress

A circular progress indicator with optional center text.

## Basic Usage

```c
ZuiWidget *progress = zui_circularprogress_new();
zui_circularprogress_set_value(ZUI_CIRCULAR_PROGRESS(progress), 0.75f);

zui_panel_add_child(ZUI_PANEL(panel), progress);
```

## Setting Progress

```c
// Progress is 0.0 to 1.0
zui_circularprogress_set_value(ZUI_CIRCULAR_PROGRESS(cp), 0.5f);
```

## Sizing

```c
// Overall diameter
zui_circularprogress_set_size(ZUI_CIRCULAR_PROGRESS(cp), 120);

// Ring thickness
zui_circularprogress_set_thickness(ZUI_CIRCULAR_PROGRESS(cp), 8);
```

## Colors

```c
zui_circularprogress_set_colors(ZUI_CIRCULAR_PROGRESS(cp),
    ZUI_COLOR_HEX(0x34495e),  // Track color
    ZUI_COLOR_HEX(0x3498db)); // Fill color
```

## Center Text

```c
// Display percentage
zui_circularprogress_set_text(ZUI_CIRCULAR_PROGRESS(cp), "75%");

// Or custom text
zui_circularprogress_set_text(ZUI_CIRCULAR_PROGRESS(cp), "3/4");

// Text styling
zui_circularprogress_set_text_color(ZUI_CIRCULAR_PROGRESS(cp), ZUI_COLOR_HEX(0xffffff));
zui_circularprogress_set_text_size(ZUI_CIRCULAR_PROGRESS(cp), 24);
```

## Click Handling

```c
void on_click(ZuiWidget *widget, uint32_t button, void *data)
{
    printf("Circular progress clicked\n");
}

zui_circularprogress_on_click(ZUI_CIRCULAR_PROGRESS(cp), on_click, NULL);
```

## Examples

### Completion Indicator

```c
typedef struct {
    ZuiWidget *progress;
    int completed;
    int total;
} TaskCounter;

void update_task_counter(TaskCounter *tc)
{
    float value = (float)tc->completed / tc->total;
    zui_circularprogress_set_value(ZUI_CIRCULAR_PROGRESS(tc->progress), value);
    
    char text[16];
    snprintf(text, sizeof(text), "%d/%d", tc->completed, tc->total);
    zui_circularprogress_set_text(ZUI_CIRCULAR_PROGRESS(tc->progress), text);
    
    // Change color when complete
    if (tc->completed == tc->total) {
        zui_circularprogress_set_colors(ZUI_CIRCULAR_PROGRESS(tc->progress),
            ZUI_COLOR_HEX(0x27ae60),
            ZUI_COLOR_HEX(0x2ecc71));
    }
}

ZuiWidget *create_task_counter(TaskCounter *tc, int total)
{
    tc->completed = 0;
    tc->total = total;
    
    tc->progress = zui_circularprogress_new();
    zui_circularprogress_set_size(ZUI_CIRCULAR_PROGRESS(tc->progress), 100);
    zui_circularprogress_set_thickness(ZUI_CIRCULAR_PROGRESS(tc->progress), 8);
    zui_circularprogress_set_colors(ZUI_CIRCULAR_PROGRESS(tc->progress),
        ZUI_COLOR_HEX(0x2c3e50),
        ZUI_COLOR_HEX(0x3498db));
    
    zui_circularprogress_set_text_size(ZUI_CIRCULAR_PROGRESS(tc->progress), 20);
    zui_circularprogress_set_text_color(ZUI_CIRCULAR_PROGRESS(tc->progress), 
        ZUI_COLOR_HEX(0xffffff));
    
    update_task_counter(tc);
    
    return tc->progress;
}
```

### Score Display

```c
ZuiWidget *create_score_display(int score, int max_score)
{
    float percentage = (float)score / max_score;
    
    ZuiWidget *cp = zui_circularprogress_new();
    zui_circularprogress_set_size(ZUI_CIRCULAR_PROGRESS(cp), 150);
    zui_circularprogress_set_thickness(ZUI_CIRCULAR_PROGRESS(cp), 12);
    zui_circularprogress_set_value(ZUI_CIRCULAR_PROGRESS(cp), percentage);
    
    // Color based on score
    ZuiColor fill;
    if (percentage >= 0.8f) {
        fill = ZUI_COLOR_HEX(0x2ecc71);  // Green - excellent
    } else if (percentage >= 0.6f) {
        fill = ZUI_COLOR_HEX(0xf1c40f);  // Yellow - good
    } else if (percentage >= 0.4f) {
        fill = ZUI_COLOR_HEX(0xe67e22);  // Orange - average
    } else {
        fill = ZUI_COLOR_HEX(0xe74c3c);  // Red - needs work
    }
    
    zui_circularprogress_set_colors(ZUI_CIRCULAR_PROGRESS(cp),
        ZUI_COLOR_HEX(0x2c3e50), fill);
    
    char text[8];
    snprintf(text, sizeof(text), "%d%%", (int)(percentage * 100));
    zui_circularprogress_set_text(ZUI_CIRCULAR_PROGRESS(cp), text);
    zui_circularprogress_set_text_size(ZUI_CIRCULAR_PROGRESS(cp), 28);
    zui_circularprogress_set_text_color(ZUI_CIRCULAR_PROGRESS(cp), fill);
    
    return cp;
}
```

### Dashboard Stats

```c
typedef struct {
    const char *label;
    float value;
    ZuiColor color;
} Stat;

ZuiWidget *create_stats_row(Stat *stats, int count)
{
    ZuiWidget *row = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(row), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(row), 32);
    zui_panel_set_alignment(ZUI_PANEL(row), ZUI_ALIGN_CENTER, ZUI_ALIGN_CENTER);
    
    for (int i = 0; i < count; i++) {
        ZuiWidget *stat_panel = zui_panel_new();
        zui_panel_set_layout(ZUI_PANEL(stat_panel), ZUI_LAYOUT_VERTICAL);
        zui_panel_set_spacing(ZUI_PANEL(stat_panel), 8);
        zui_panel_set_alignment(ZUI_PANEL(stat_panel), ZUI_ALIGN_CENTER, ZUI_ALIGN_CENTER);
        
        ZuiWidget *cp = zui_circularprogress_new();
        zui_circularprogress_set_size(ZUI_CIRCULAR_PROGRESS(cp), 80);
        zui_circularprogress_set_thickness(ZUI_CIRCULAR_PROGRESS(cp), 6);
        zui_circularprogress_set_value(ZUI_CIRCULAR_PROGRESS(cp), stats[i].value);
        zui_circularprogress_set_colors(ZUI_CIRCULAR_PROGRESS(cp),
            ZUI_COLOR_HEX(0x2c3e50), stats[i].color);
        
        char text[8];
        snprintf(text, sizeof(text), "%.0f%%", stats[i].value * 100);
        zui_circularprogress_set_text(ZUI_CIRCULAR_PROGRESS(cp), text);
        zui_circularprogress_set_text_size(ZUI_CIRCULAR_PROGRESS(cp), 16);
        zui_circularprogress_set_text_color(ZUI_CIRCULAR_PROGRESS(cp), 
            ZUI_COLOR_HEX(0xffffff));
        
        ZuiWidget *label = zui_label_new(stats[i].label);
        zui_label_set_size(ZUI_LABEL(label), 14);
        zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR_HEX(0x95a5a6));
        
        zui_panel_add_child(ZUI_PANEL(stat_panel), cp);
        zui_panel_add_child(ZUI_PANEL(stat_panel), label);
        zui_panel_add_child(ZUI_PANEL(row), stat_panel);
    }
    
    return row;
}

// Usage
Stat stats[] = {
    {"CPU", 0.65f, ZUI_COLOR_HEX(0x3498db)},
    {"Memory", 0.82f, ZUI_COLOR_HEX(0xe74c3c)},
    {"Disk", 0.45f, ZUI_COLOR_HEX(0x2ecc71)},
    {"Network", 0.33f, ZUI_COLOR_HEX(0x9b59b6)},
};
ZuiWidget *stats_ui = create_stats_row(stats, 4);
```

### Timer Display

```c
typedef struct {
    ZuiWidget *progress;
    int total_seconds;
    int remaining_seconds;
} Timer;

void update_timer(Timer *timer)
{
    float progress = 1.0f - ((float)timer->remaining_seconds / timer->total_seconds);
    zui_circularprogress_set_value(ZUI_CIRCULAR_PROGRESS(timer->progress), progress);
    
    int mins = timer->remaining_seconds / 60;
    int secs = timer->remaining_seconds % 60;
    char text[8];
    snprintf(text, sizeof(text), "%d:%02d", mins, secs);
    zui_circularprogress_set_text(ZUI_CIRCULAR_PROGRESS(timer->progress), text);
}

ZuiWidget *create_timer(Timer *timer, int seconds)
{
    timer->total_seconds = seconds;
    timer->remaining_seconds = seconds;
    
    timer->progress = zui_circularprogress_new();
    zui_circularprogress_set_size(ZUI_CIRCULAR_PROGRESS(timer->progress), 200);
    zui_circularprogress_set_thickness(ZUI_CIRCULAR_PROGRESS(timer->progress), 10);
    zui_circularprogress_set_colors(ZUI_CIRCULAR_PROGRESS(timer->progress),
        ZUI_COLOR_HEX(0x2c3e50),
        ZUI_COLOR_HEX(0x3498db));
    zui_circularprogress_set_text_size(ZUI_CIRCULAR_PROGRESS(timer->progress), 48);
    zui_circularprogress_set_text_color(ZUI_CIRCULAR_PROGRESS(timer->progress), 
        ZUI_COLOR_HEX(0xffffff));
    
    update_timer(timer);
    
    return timer->progress;
}
```

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_circularprogress_new()` | Create widget, returns `ZuiWidget *` |
| `zui_circularprogress_create()` | Create widget, returns `ZuiCircularProgress *` |

### Value

| Function | Description |
|----------|-------------|
| `zui_circularprogress_set_value(cp, value)` | Set progress (0.0 - 1.0) |

### Appearance

| Function | Description |
|----------|-------------|
| `zui_circularprogress_set_size(cp, size)` | Set diameter |
| `zui_circularprogress_set_thickness(cp, thickness)` | Set ring thickness |
| `zui_circularprogress_set_colors(cp, track, fill)` | Set track and fill colors |

### Text

| Function | Description |
|----------|-------------|
| `zui_circularprogress_set_text(cp, text)` | Set center text |
| `zui_circularprogress_set_text_color(cp, color)` | Set text color |
| `zui_circularprogress_set_text_size(cp, size)` | Set text size |

### Events

| Function | Description |
|----------|-------------|
| `zui_circularprogress_on_click(cp, callback, data)` | Click handler |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_CIRCULAR_PROGRESS(w)` | Check if widget is a circular progress |
| `ZUI_CIRCULAR_PROGRESS(w)` | Downcast to `ZuiCircularProgress *` (or NULL) |

---

**See Also:** [ProgressBar](progressbar.md) · [Charts](charts.md) · [Widget Overview](overview.md)
