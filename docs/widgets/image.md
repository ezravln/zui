# Image

Displays an image from file or memory.

## Basic Usage

```c
// From file path
ZuiWidget *img = zui_image_new("assets/photo.png");
zui_panel_add_child(ZUI_PANEL(panel), img);
```

## Loading Images

### From File

```c
ZuiWidget *img = zui_image_new("/path/to/image.png");

// Or using typed create
ZuiImage *img = zui_image_create("assets/logo.png");
```

### From Memory

```c
unsigned char *data = load_image_data(...);
int data_len = ...;

ZuiImage *img = zui_image_create_from_memory(data, data_len);
```

## Sizing

```c
// Fixed size
zui_image_set_size(ZUI_IMAGE(img), 200, 150);

// Using widget sizing
zui_widget_set_size(img, 200, 150);
```

## Visibility

```c
zui_image_set_visible(ZUI_IMAGE(img), false);  // Hide
zui_image_set_visible(ZUI_IMAGE(img), true);   // Show
```

## Examples

### Profile Avatar

```c
ZuiWidget *create_avatar(const char *image_path, float size)
{
    ZuiWidget *container = zui_panel_new();
    zui_widget_set_size(container, size, size);
    zui_panel_set_corner_radius(ZUI_PANEL(container), size / 2);  // Circular
    zui_panel_set_background(ZUI_PANEL(container), ZUI_COLOR_HEX(0x34495e));
    
    ZuiWidget *img = zui_image_new(image_path);
    zui_image_set_size(ZUI_IMAGE(img), size, size);
    
    zui_panel_add_child(ZUI_PANEL(container), img);
    
    return container;
}

// Usage
ZuiWidget *avatar = create_avatar("assets/user.png", 64);
```

### Image Card

```c
ZuiWidget *create_image_card(const char *image_path, 
                              const char *title, const char *description)
{
    ZuiWidget *card = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(card), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_background(ZUI_PANEL(card), ZUI_COLOR_HEX(0x2c3e50));
    zui_panel_set_corner_radius(ZUI_PANEL(card), 12);
    zui_widget_set_size(card, 280, 0);
    
    // Image
    ZuiWidget *img = zui_image_new(image_path);
    zui_image_set_size(ZUI_IMAGE(img), 280, 180);
    
    // Content
    ZuiWidget *content = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(content), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(content), 8);
    zui_panel_set_padding(ZUI_PANEL(content), 16, 16, 16, 16);
    
    ZuiWidget *title_lbl = zui_label_new(title);
    zui_label_set_size(ZUI_LABEL(title_lbl), 18);
    zui_label_set_color(ZUI_LABEL(title_lbl), ZUI_COLOR_HEX(0xffffff));
    
    ZuiWidget *desc_lbl = zui_label_new(description);
    zui_label_set_size(ZUI_LABEL(desc_lbl), 14);
    zui_label_set_color(ZUI_LABEL(desc_lbl), ZUI_COLOR_HEX(0x95a5a6));
    
    zui_panel_add_child(ZUI_PANEL(content), title_lbl);
    zui_panel_add_child(ZUI_PANEL(content), desc_lbl);
    
    zui_panel_add_child(ZUI_PANEL(card), img);
    zui_panel_add_child(ZUI_PANEL(card), content);
    
    return card;
}
```

### Image Gallery with Thumbnails

```c
typedef struct {
    ZuiWidget *main_image;
    const char **paths;
    int count;
    int selected;
} Gallery;

void on_thumbnail_click(ZuiWidget *widget, void *data)
{
    Gallery *gallery = (Gallery *)data;
    // Update selected would need per-thumbnail tracking
}

ZuiWidget *create_gallery(Gallery *gallery, const char **paths, int count)
{
    gallery->paths = paths;
    gallery->count = count;
    gallery->selected = 0;
    
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_VERTICAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 12);
    
    // Main image
    gallery->main_image = zui_image_new(paths[0]);
    zui_image_set_size(ZUI_IMAGE(gallery->main_image), 400, 300);
    
    // Thumbnails
    ZuiWidget *thumbs = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(thumbs), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(thumbs), 8);
    
    for (int i = 0; i < count; i++) {
        ZuiWidget *thumb_container = zui_panel_new();
        zui_widget_set_size(thumb_container, 60, 60);
        zui_panel_set_background(ZUI_PANEL(thumb_container), 
            i == 0 ? ZUI_COLOR_HEX(0x3498db) : ZUI_COLOR_HEX(0x2c3e50));
        zui_panel_set_corner_radius(ZUI_PANEL(thumb_container), 4);
        zui_panel_set_padding(ZUI_PANEL(thumb_container), 2, 2, 2, 2);
        zui_set_cursor(thumb_container, ZUI_CURSOR_POINTER);
        
        ZuiWidget *thumb = zui_image_new(paths[i]);
        zui_image_set_size(ZUI_IMAGE(thumb), 56, 56);
        
        zui_panel_add_child(ZUI_PANEL(thumb_container), thumb);
        zui_panel_add_child(ZUI_PANEL(thumbs), thumb_container);
    }
    
    zui_panel_add_child(ZUI_PANEL(container), gallery->main_image);
    zui_panel_add_child(ZUI_PANEL(container), thumbs);
    
    return container;
}
```

### Logo with Fallback

```c
ZuiWidget *create_logo(const char *logo_path, const char *app_name)
{
    ZuiWidget *container = zui_panel_new();
    zui_panel_set_layout(ZUI_PANEL(container), ZUI_LAYOUT_HORIZONTAL);
    zui_panel_set_spacing(ZUI_PANEL(container), 12);
    zui_panel_set_alignment(ZUI_PANEL(container), ZUI_ALIGN_START, ZUI_ALIGN_CENTER);
    
    // Try to load logo
    ZuiWidget *logo = zui_image_new(logo_path);
    if (logo) {
        zui_image_set_size(ZUI_IMAGE(logo), 32, 32);
        zui_panel_add_child(ZUI_PANEL(container), logo);
    } else {
        // Fallback: colored square with initial
        ZuiWidget *fallback = zui_panel_new();
        zui_widget_set_size(fallback, 32, 32);
        zui_panel_set_background(ZUI_PANEL(fallback), ZUI_COLOR_HEX(0x3498db));
        zui_panel_set_corner_radius(ZUI_PANEL(fallback), 6);
        zui_panel_set_alignment(ZUI_PANEL(fallback), ZUI_ALIGN_CENTER, ZUI_ALIGN_CENTER);
        
        char initial[2] = {app_name[0], '\0'};
        ZuiWidget *lbl = zui_label_new(initial);
        zui_label_set_size(ZUI_LABEL(lbl), 18);
        zui_label_set_color(ZUI_LABEL(lbl), ZUI_COLOR_HEX(0xffffff));
        
        zui_panel_add_child(ZUI_PANEL(fallback), lbl);
        zui_panel_add_child(ZUI_PANEL(container), fallback);
    }
    
    // App name
    ZuiWidget *name = zui_label_new(app_name);
    zui_label_set_size(ZUI_LABEL(name), 20);
    zui_label_set_color(ZUI_LABEL(name), ZUI_COLOR_HEX(0xffffff));
    
    zui_panel_add_child(ZUI_PANEL(container), name);
    
    return container;
}
```

## Supported Formats

ZUI supports common image formats via stb_image:

- PNG
- JPEG
- BMP
- GIF (first frame only)
- TGA

## API Reference

### Creation

| Function | Description |
|----------|-------------|
| `zui_image_new(path)` | Create from file, returns `ZuiWidget *` |
| `zui_image_create(path)` | Create from file, returns `ZuiImage *` |
| `zui_image_create_from_memory(data, len)` | Create from memory buffer |

### Configuration

| Function | Description |
|----------|-------------|
| `zui_image_set_size(img, width, height)` | Set display size |
| `zui_image_set_visible(img, visible)` | Show/hide image |

### Type Macros

| Macro | Description |
|-------|-------------|
| `ZUI_IS_IMAGE(w)` | Check if widget is an image |
| `ZUI_IMAGE(w)` | Downcast to `ZuiImage *` (or NULL) |

## Notes

- Images are loaded asynchronously where possible
- Large images are automatically scaled for GPU limits
- Memory images must remain valid or be copied

---

**See Also:** [Icon](icon.md) · [Panel](panel.md) · [Widget Overview](overview.md)
