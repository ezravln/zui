# GridView

A container that arranges children in a grid layout.

## Basic Usage

```c
ZuiWidget *grid = zui_gridview_new(3);  // 3 columns

zui_gridview_add_child(ZUI_GRIDVIEW(grid), item1);
zui_gridview_add_child(ZUI_GRIDVIEW(grid), item2);
zui_gridview_add_child(ZUI_GRIDVIEW(grid), item3);
// Items flow left-to-right, wrapping to next row

zui_panel_add_child(ZUI_PANEL(panel), grid);
```

## Configuration

```c
// Number of columns
zui_gridview_set_columns(ZUI_GRIDVIEW(grid), 4);

// Gap between cells
zui_gridview_set_gap(ZUI_GRIDVIEW(grid), 16, 16);  // x, y

// Outer padding
zui_gridview_set_padding(ZUI_GRIDVIEW(grid), 20);

// Fixed cell size (optional)
zui_gridview_set_cell_size(ZUI_GRIDVIEW(grid), 150, 150);
```

## Managing Children

```c
// Add
zui_gridview_add_child(ZUI_GRIDVIEW(grid), widget);

// Remove
zui_gridview_remove_child(ZUI_GRIDVIEW(grid), widget);

// Clear all
zui_gridview_clear(ZUI_GRIDVIEW(grid));
```

## Examples

### Photo Gallery

```c
ZuiWidget *create_photo_gallery(const char **image_paths, int count)
{
    ZuiWidget *scroll = zui_scrollview_new();
    zui_widget_set_fill(scroll, true, true);
    
    ZuiWidget *grid = zui_gridview_new(4);
    zui_gridview_set_gap(ZUI_GRIDVIEW(grid), 8, 8);
    zui_gridview_set_padding(ZUI_GRIDVIEW(grid), 16);
    
    for (int i = 0; i < count; i++) {
        ZuiWidget *cell = zui_panel_new();
        zui_panel_set_background(ZUI_PANEL(cell), ZUI_COLOR_HEX(0x2c3e50));
        zui_panel_set_corner_radius(ZUI_PANEL(cell), 8);
        zui_widget_set_size(cell, 0, 150);
        
        ZuiWidget *img = zui_image_new(image_paths[i]);
        zui_image_set_size(ZUI_IMAGE(img), 150, 150);
        
        zui_panel_add_child(ZUI_PANEL(cell), img);
        zui_gridview_add_child(ZUI_GRIDVIEW(grid), cell);
    }
    
    zui_scrollview_set_content(ZUI_SCROLLVIEW(scroll), (ZuiWidget *)grid);
    
    return scroll;
}
```

### App Grid (Launcher Style)

```c
typedef struct {
    const char *name;
    const char *icon_path;
    void (*on_click)(void *data);
    void *data;
} AppEntry;

void on_app_click(ZuiWidget *widget, void *data)
{
    AppEntry *app = (AppEntry *)data;
    if (app->on_click) {
        app->on_click(app->data);
    }
}

ZuiWidget *create_app_grid(AppEntry *apps, int count)
{
    ZuiWidget *grid = zui_gridview_new(5);
    zui_gridview_set_gap(ZUI_GRIDVIEW(grid), 24, 24);
    zui_gridview_set_padding(ZUI_GRIDVIEW(grid), 32);
    
    for (int i = 0; i < count; i++) {
        ZuiWidget *cell = zui_panel_new();
        zui_panel_set_layout(ZUI_PANEL(cell), ZUI_LAYOUT_VERTICAL);
        zui_panel_set_spacing(ZUI_PANEL(cell), 8);
        zui_panel_set_alignment(ZUI_PANEL(cell), ZUI_ALIGN_CENTER, ZUI_ALIGN_CENTER);
        zui_widget_set_size(cell, 80, 100);
        zui_set_cursor(cell, ZUI_CURSOR_POINTER);
        
        // Icon
        ZuiWidget *icon = zui_image_new(apps[i].icon_path);
        zui_image_set_size(ZUI_IMAGE(icon), 48, 48);
        
        // Name
        ZuiWidget *name = zui_label_new(apps[i].name);
        zui_label_set_size(ZUI_LABEL(name), 12);
        zui_label_set_color(ZUI_LABEL(name), ZUI_COLOR_HEX(0xffffff));
        
        zui_panel_add_child(ZUI_PANEL(cell), icon);
        zui_panel_add_child(ZUI_PANEL(cell), name);
        
        // Click handling would need custom setup
        
        zui_gridview_add_child(ZUI_GRIDVIEW(grid), cell);
    }
    
    return grid;
}
```

### Product Catalog

```c
typedef struct {
    const char *name;
    const char *image;
    float price;
    bool in_stock;
} Product;

ZuiWidget *create_product_card(Product *product)
{
    ZuiWidget *card = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(card), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(card), 8);
    zui_panel_set_padding(ZUI_PANEL(card), 0, 0, 12, 0);
    zui_panel_set_background(ZUI_PANEL(card), ZUI_COLOR_HEX(0x2c3e50));
    zui_panel_set_corner_radius(ZUI_PANEL(card), 8);
    
    // Image
    ZuiWidget *img = zui_image_new(product->image);
    zui_image_set_size(ZUI_IMAGE(img), 180, 140);
    
    // Info container
    ZuiWidget *info = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(info), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(info), 4);
    zui_panel_set_padding(ZUI_PANEL(info), 0, 12, 0, 12);
    
    // Name
    ZuiWidget *name = zui_label_new(product->name);
    zui_label_set_color(ZUI_LABEL(name), ZUI_COLOR_HEX(0xffffff));
    
    // Price
    char price_str[16];
    snprintf(price_str, sizeof(price_str), "$%.2f", product->price);
    ZuiWidget *price = zui_label_new(price_str);
    zui_label_set_size(ZUI_LABEL(price), 18);
    zui_label_set_color(ZUI_LABEL(price), ZUI_COLOR_HEX(0x3498db));
    
    // Stock status
    ZuiWidget *stock = zui_label_new(product->in_stock ? "In Stock" : "Out of Stock");
    zui_label_set_size(ZUI_LABEL(stock), 12);
    zui_label_set_color(ZUI_LABEL(stock), 
        product->in_stock ? ZUI_COLOR_HEX(0x2ecc71) : ZUI_COLOR_HEX(0xe74c3c));
    
    zui_panel_add_child(ZUI_PANEL(info), name);
    zui_panel_add_child(ZUI_PANEL(info), price);
    zui_panel_add_child(ZUI_PANEL(info), stock);
    
    zui_panel_add_child(ZUI_PANEL(card), img);
    zui_panel_add_child(ZUI_PANEL(card), info);
    
    return card;
}

ZuiWidget *create_product_catalog(Product *products, int count)
{
    ZuiWidget *scroll = zui_scrollview_new();
    zui_widget_set_fill(scroll, true, true);
    
    ZuiWidget *grid = zui_gridview_new(4);
    zui_gridview_set_gap(ZUI_GRIDVIEW(grid), 16, 16);
    zui_gridview_set_padding(ZUI_GRIDVIEW(grid), 24);
    
    for (int i = 0; i < count; i++) {
        zui_gridview_add_child(ZUI_GRIDVIEW(grid), 
            create_product_card(&products[i]));
    }
    
    zui_scrollview_set_content(ZUI_SCROLLVIEW(scroll), (ZuiWidget *)grid);
    
    return scroll;
}
```

### Dashboard Tiles

```c
typedef struct {
    const char *title;
    const char *value;
    const char *subtitle;
    uint32_t accent_color;
} DashboardTile;

ZuiWidget *create_tile(DashboardTile *tile)
{
    ZuiWidget *card = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(card), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(card), 8);
    zui_panel_set_padding(ZUI_PANEL(card), 20, 20, 20, 20);
    zui_panel_set_background(ZUI_PANEL(card), ZUI_COLOR_HEX(0x2c3e50));
    zui_panel_set_corner_radius(ZUI_PANEL(card), 12);
    
    // Accent bar
    ZuiWidget *accent = zui_panel_new();
    zui_widget_set_size(accent, 40, 4);
    zui_panel_set_background(ZUI_PANEL(accent), ZUI_COLOR_HEX(tile->accent_color));
    zui_panel_set_corner_radius(ZUI_PANEL(accent), 2);
    
    // Title
    ZuiWidget *title = zui_label_new(tile->title);
    zui_label_set_size(ZUI_LABEL(title), 14);
    zui_label_set_color(ZUI_LABEL(title), ZUI_COLOR_HEX(0x7f8c8d));
    
    // Value
    ZuiWidget *value = zui_label_new(tile->value);
    zui_label_set_size(ZUI_LABEL(value), 32);
    zui_label_set_color(ZUI_LABEL(value), ZUI_COLOR_HEX(0xffffff));
    
    // Subtitle
    ZuiWidget *subtitle = zui_label_new(tile->subtitle);
    zui_label_set_size(ZUI_LABEL(subtitle), 12);
    zui_label_set_color(ZUI_LABEL(subtitle), ZUI_COLOR_HEX(0x7f8c8d));
    
    zui_panel_add_child(ZUI_PANEL(card), accent);
    zui_panel_add_child(ZUI_PANEL(card), title);
    zui_panel_add_child(ZUI_PANEL(card), value);
    zui_panel_add_child(ZUI_PANEL(card), subtitle);
    
    return card;
}

ZuiWidget *create_dashboard(void)
{
    DashboardTile tiles[] = {
        {"Revenue", "$12,450", "+12% from last month", 0x2ecc71},
        {"Orders", "156", "23 pending", 0x3498db},
        {"Users", "2,847", "+89 this week", 0x9b59b6},
        {"Conversion", "3.2%", "Above target", 0xe74c3c},
    };
    
    ZuiWidget *grid = zui_gridview_new(2);
    zui_gridview_set_gap(ZUI_GRIDVIEW(grid), 16, 16);
    zui_gridview_set_padding(ZUI_GRIDVIEW(grid), 24);
    
    for (int i = 0; i < 4; i++) {
        zui_gridview_add_child(ZUI_GRIDVIEW(grid), create_tile(&tiles[i]));
    }
    
    return grid;
}
```

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_gridview_new(columns)` | Create grid, returns `ZuiWidget *` |
| `zui_gridview_create(columns)` | Create grid, returns `ZuiGridView *` |

### Configuration

| Function | Description |
|----------|-------------|
| `zui_gridview_set_columns(gv, columns)` | Set column count |
| `zui_gridview_set_gap(gv, x, y)` | Set gap between cells |
| `zui_gridview_set_padding(gv, padding)` | Set outer padding |
| `zui_gridview_set_cell_size(gv, width, height)` | Set fixed cell size |

### Children

| Function | Description |
|----------|-------------|
| `zui_gridview_add_child(gv, widget)` | Add child widget |
| `zui_gridview_remove_child(gv, widget)` | Remove child widget |
| `zui_gridview_clear(gv)` | Remove all children |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_GRIDVIEW(w)` | Check if widget is a grid view |
| `ZUI_GRIDVIEW(w)` | Downcast to `ZuiGridView *` (or NULL) |

## Notes

- Children fill columns left-to-right before wrapping
- Grid automatically calculates row count based on children
- Best used inside a ScrollView for large grids

---

**See Also:** [Panel](panel.md) · [ScrollView](scrollview.md) · [Layout Guide](../guides/layout.md)
