# Charts

ZUI includes several chart widgets for data visualization: Pie Chart, Bar Chart, and Line Chart.

## Pie Chart

Displays data as proportional slices of a circle.

### Basic Usage

```c
ZuiWidget *chart = zui_piechart_new();

// Add slices with value and color
zui_piechart_add_slice(ZUI_PIECHART(chart), 30, ZUI_COLOR_HEX(0x3498db), "Blue");
zui_piechart_add_slice(ZUI_PIECHART(chart), 45, ZUI_COLOR_HEX(0x2ecc71), "Green");
zui_piechart_add_slice(ZUI_PIECHART(chart), 25, ZUI_COLOR_HEX(0xe74c3c), "Red");

zui_panel_add_child(ZUI_PANEL(panel), chart);
```

### Styling

```c
// Size
zui_piechart_set_size(ZUI_PIECHART(chart), 200);

// Donut style (center hole)
zui_piechart_set_inner_radius(ZUI_PIECHART(chart), 60);
```

### Example: Category Breakdown

```c
typedef struct {
    const char *name;
    float value;
    uint32_t color;
} Category;

ZuiWidget *create_category_chart(Category *cats, int count)
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 24);
    
    // Chart
    ZuiWidget *chart = zui_piechart_new();
    zui_piechart_set_size(ZUI_PIECHART(chart), 180);
    zui_piechart_set_inner_radius(ZUI_PIECHART(chart), 50);
    
    for (int i = 0; i < count; i++) {
        zui_piechart_add_slice(ZUI_PIECHART(chart),
            cats[i].value, ZUI_COLOR_HEX(cats[i].color), cats[i].name);
    }
    
    // Legend
    ZuiWidget *legend = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(legend), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(legend), 8);
    
    float total = 0;
    for (int i = 0; i < count; i++) total += cats[i].value;
    
    for (int i = 0; i < count; i++) {
        ZuiWidget *row = zui_panel_new();
        zui_panel_set_layout(ZUI_PANEL(row), ZUI_LAYOUT_HORIZONTAL);
        zui_panel_set_spacing(ZUI_PANEL(row), 8);
        zui_panel_set_alignment(ZUI_PANEL(row), ZUI_ALIGN_START, ZUI_ALIGN_CENTER);
        
        // Color dot
        ZuiWidget *dot = zui_panel_new();
        zui_widget_set_size(dot, 12, 12);
        zui_panel_set_background(ZUI_PANEL(dot), ZUI_COLOR_HEX(cats[i].color));
        zui_panel_set_corner_radius(ZUI_PANEL(dot), 6);
        
        // Label with percentage
        char text[64];
        snprintf(text, sizeof(text), "%s (%.1f%%)", 
            cats[i].name, (cats[i].value / total) * 100);
        ZuiWidget *label = zui_label_new(text);
        zui_label_set_size(ZUI_LABEL(label), 14);
        zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR_HEX(0xbdc3c7));
        
        zui_panel_add_child(ZUI_PANEL(row), dot);
        zui_panel_add_child(ZUI_PANEL(row), label);
        zui_panel_add_child(ZUI_PANEL(legend), row);
    }
    
    zui_panel_add_child(ZUI_PANEL(container), chart);
    zui_panel_add_child(ZUI_PANEL(container), legend);
    
    return container;
}

// Usage
Category categories[] = {
    {"Development", 45, 0x3498db},
    {"Marketing", 25, 0x2ecc71},
    {"Operations", 20, 0xe74c3c},
    {"Research", 10, 0x9b59b6},
};
ZuiWidget *chart = create_category_chart(categories, 4);
```

---

## Bar Chart

Displays data as horizontal or vertical bars.

### Basic Usage

```c
ZuiWidget *chart = zui_barchart_new();

// Add bars
zui_barchart_add_bar(ZUI_BARCHART(chart), 85, "Jan", ZUI_COLOR_HEX(0x3498db));
zui_barchart_add_bar(ZUI_BARCHART(chart), 92, "Feb", ZUI_COLOR_HEX(0x3498db));
zui_barchart_add_bar(ZUI_BARCHART(chart), 78, "Mar", ZUI_COLOR_HEX(0x3498db));
zui_barchart_add_bar(ZUI_BARCHART(chart), 95, "Apr", ZUI_COLOR_HEX(0x3498db));

zui_widget_set_size(chart, 400, 200);
zui_panel_add_child(ZUI_PANEL(panel), chart);
```

### Styling

```c
// Bar appearance
zui_barchart_set_bar_width(ZUI_BARCHART(chart), 40);
zui_barchart_set_bar_spacing(ZUI_BARCHART(chart), 10);
zui_barchart_set_corner_radius(ZUI_BARCHART(chart), 4);

// Axis and labels
zui_barchart_set_label_color(ZUI_BARCHART(chart), ZUI_COLOR_HEX(0x95a5a6));
zui_barchart_show_values(ZUI_BARCHART(chart), true);
```

### Example: Monthly Comparison

```c
typedef struct {
    const char *month;
    float current;
    float previous;
} MonthlyData;

ZuiWidget *create_comparison_chart(MonthlyData *data, int count)
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 16);
    
    // Legend
    ZuiWidget *legend = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(legend), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(legend), 24);
    
    // Current year
    ZuiWidget *current_legend = create_legend_item("This Year", 0x3498db);
    ZuiWidget *prev_legend = create_legend_item("Last Year", 0x7f8c8d);
    
    zui_panel_add_child(ZUI_PANEL(legend), current_legend);
    zui_panel_add_child(ZUI_PANEL(legend), prev_legend);
    
    // Chart
    ZuiWidget *chart = zui_barchart_new();
    zui_widget_set_size(chart, 500, 250);
    zui_barchart_set_bar_width(ZUI_BARCHART(chart), 20);
    
    for (int i = 0; i < count; i++) {
        // Previous year (lighter)
        zui_barchart_add_bar(ZUI_BARCHART(chart), 
            data[i].previous, "", ZUI_COLOR_HEX(0x7f8c8d));
        // Current year
        zui_barchart_add_bar(ZUI_BARCHART(chart), 
            data[i].current, data[i].month, ZUI_COLOR_HEX(0x3498db));
    }
    
    zui_panel_add_child(ZUI_PANEL(container), legend);
    zui_panel_add_child(ZUI_PANEL(container), chart);
    
    return container;
}
```

---

## Line Chart

Displays data as connected points over time.

### Basic Usage

```c
ZuiWidget *chart = zui_linechart_new();

// Add data points
float values[] = {10, 25, 18, 30, 22, 35, 28};
const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

zui_linechart_set_data(ZUI_LINECHART(chart), values, labels, 7);

zui_widget_set_size(chart, 400, 200);
zui_panel_add_child(ZUI_PANEL(panel), chart);
```

### Styling

```c
// Line appearance
zui_linechart_set_line_color(ZUI_LINECHART(chart), ZUI_COLOR_HEX(0x3498db));
zui_linechart_set_line_width(ZUI_LINECHART(chart), 2.0f);

// Points
zui_linechart_show_points(ZUI_LINECHART(chart), true);
zui_linechart_set_point_radius(ZUI_LINECHART(chart), 4);
zui_linechart_set_point_color(ZUI_LINECHART(chart), ZUI_COLOR_HEX(0xffffff));

// Fill under line
zui_linechart_set_fill(ZUI_LINECHART(chart), true);
zui_linechart_set_fill_color(ZUI_LINECHART(chart), ZUI_COLOR(0.2f, 0.6f, 0.86f, 0.3f));

// Grid and labels
zui_linechart_show_grid(ZUI_LINECHART(chart), true);
zui_linechart_set_grid_color(ZUI_LINECHART(chart), ZUI_COLOR_HEX(0x34495e));
zui_linechart_set_label_color(ZUI_LINECHART(chart), ZUI_COLOR_HEX(0x95a5a6));
```

### Example: Performance Dashboard

```c
typedef struct {
    float *values;
    int count;
    const char *name;
    uint32_t color;
} Series;

ZuiWidget *create_multi_line_chart(Series *series, int series_count, 
                                    const char **labels, int point_count)
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 12);
    zui_panel_set_padding(ZUI_PANEL(container), 16, 16, 16, 16);
    zui_panel_set_background(ZUI_PANEL(container), ZUI_COLOR_HEX(0x2c3e50));
    zui_panel_set_corner_radius(ZUI_PANEL(container), 8);
    
    // Title
    ZuiWidget *title = zui_label_new("Performance Metrics");
    zui_label_set_size(ZUI_LABEL(title), 18);
    zui_label_set_color(ZUI_LABEL(title), ZUI_COLOR_HEX(0xffffff));
    
    // Legend
    ZuiWidget *legend = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(legend), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(legend), 16);
    
    for (int i = 0; i < series_count; i++) {
        ZuiWidget *item = create_legend_item(series[i].name, series[i].color);
        zui_panel_add_child(ZUI_PANEL(legend), item);
    }
    
    // Charts (stacked)
    ZuiWidget *chart_container = zui_panel_new();
    // Charts would be overlaid in actual implementation
    
    for (int i = 0; i < series_count; i++) {
        ZuiWidget *chart = zui_linechart_new();
        zui_widget_set_size(chart, 450, 200);
        zui_linechart_set_data(ZUI_LINECHART(chart), 
            series[i].values, labels, point_count);
        zui_linechart_set_line_color(ZUI_LINECHART(chart), 
            ZUI_COLOR_HEX(series[i].color));
        zui_linechart_set_line_width(ZUI_LINECHART(chart), 2);
        zui_linechart_show_points(ZUI_LINECHART(chart), true);
        zui_linechart_set_point_radius(ZUI_LINECHART(chart), 3);
        
        zui_panel_add_child(ZUI_PANEL(chart_container), chart);
    }
    
    zui_panel_add_child(ZUI_PANEL(container), title);
    zui_panel_add_child(ZUI_PANEL(container), legend);
    zui_panel_add_child(ZUI_PANEL(container), chart_container);
    
    return container;
}

// Helper
ZuiWidget *create_legend_item(const char *name, uint32_t color)
{
    ZuiWidget *row = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(row), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(row), 6);
    zui_panel_set_alignment(ZUI_PANEL(row), ZUI_ALIGN_START, ZUI_ALIGN_CENTER);
    
    ZuiWidget *dot = zui_panel_new();
    zui_widget_set_size(dot, 10, 10);
    zui_panel_set_background(ZUI_PANEL(dot), ZUI_COLOR_HEX(color));
    zui_panel_set_corner_radius(ZUI_PANEL(dot), 5);
    
    ZuiWidget *label = zui_label_new(name);
    zui_label_set_size(ZUI_LABEL(label), 12);
    zui_label_set_color(ZUI_LABEL(label), ZUI_COLOR_HEX(0xbdc3c7));
    
    zui_panel_add_child(ZUI_PANEL(row), dot);
    zui_panel_add_child(ZUI_PANEL(row), label);
    
    return row;
}
```

---

## API Reference

### Pie Chart

| Function | Description |
|----------|-------------|
| `zui_piechart_new()` | Create pie chart |
| `zui_piechart_add_slice(chart, value, color, label)` | Add a slice |
| `zui_piechart_clear(chart)` | Remove all slices |
| `zui_piechart_set_size(chart, size)` | Set diameter |
| `zui_piechart_set_inner_radius(chart, radius)` | Create donut |

### Bar Chart

| Function | Description |
|----------|-------------|
| `zui_barchart_new()` | Create bar chart |
| `zui_barchart_add_bar(chart, value, label, color)` | Add a bar |
| `zui_barchart_clear(chart)` | Remove all bars |
| `zui_barchart_set_bar_width(chart, width)` | Set bar width |
| `zui_barchart_set_bar_spacing(chart, spacing)` | Set spacing |
| `zui_barchart_show_values(chart, show)` | Show values on bars |

### Line Chart

| Function | Description |
|----------|-------------|
| `zui_linechart_new()` | Create line chart |
| `zui_linechart_set_data(chart, values, labels, count)` | Set data points |
| `zui_linechart_set_line_color(chart, color)` | Line color |
| `zui_linechart_set_line_width(chart, width)` | Line thickness |
| `zui_linechart_show_points(chart, show)` | Show data points |
| `zui_linechart_set_fill(chart, fill)` | Fill area under line |
| `zui_linechart_show_grid(chart, show)` | Show background grid |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_PIECHART(w)` | Downcast to `ZuiPieChart *` |
| `ZUI_BARCHART(w)` | Downcast to `ZuiBarChart *` |
| `ZUI_LINECHART(w)` | Downcast to `ZuiLineChart *` |

---

**See Also:** [CircularProgress](circularprogress.md) · [ProgressBar](progressbar.md) · [Widget Overview](overview.md)
