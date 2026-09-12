# Installation

This guide covers how to build and install ZUI on your system.

## Prerequisites

### Required Dependencies

<div class="tabs">

#### Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config \
    libwayland-dev libxkbcommon-dev libegl1-mesa-dev \
    libgles2-mesa-dev libfreetype6-dev
```

#### Fedora

```bash
sudo dnf install gcc cmake pkgconfig \
    wayland-devel libxkbcommon-devel mesa-libEGL-devel \
    mesa-libGLES-devel freetype-devel
```

#### Arch Linux

```bash
sudo pacman -S base-devel cmake pkgconf \
    wayland libxkbcommon mesa freetype2
```

</div>

### Optional Dependencies

For audio support:

```bash
# Ubuntu/Debian
sudo apt install libmpv-dev

# Fedora
sudo dnf install mpv-libs-devel

# Arch Linux
sudo pacman -S mpv
```

## Building from Source

### Clone the Repository

```bash
git clone https://github.com/ezravln/zui.git
cd zui
```

### Configure with CMake

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

#### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | `Debug` | Build type: `Debug`, `Release`, `RelWithDebInfo` |
| `ZUI_BUILD_EXAMPLES` | `ON` | Build example applications |
| `ZUI_BUILD_TESTS` | `OFF` | Build test suite |
| `ZUI_ENABLE_AUDIO` | `ON` | Enable audio support (requires libmpv) |

Example with options:

```bash
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DZUI_BUILD_EXAMPLES=ON \
    -DZUI_ENABLE_AUDIO=ON
```

### Build

```bash
cmake --build build -j$(nproc)
```

### Install (Optional)

```bash
sudo cmake --install build
```

This installs:
- Library: `/usr/local/lib/libzui.a`
- Headers: `/usr/local/include/zui/`
- CMake config: `/usr/local/lib/cmake/zui/`

## Using ZUI in Your Project

### With CMake (Recommended)

If ZUI is installed system-wide:

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_app)

find_package(zui REQUIRED)

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE zui::zui)
```

### As a Subdirectory

Clone or add ZUI as a git submodule:

```bash
git submodule add https://github.com/ezravln/zui.git external/zui
```

Then in your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_app)

add_subdirectory(external/zui)

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE zui)
```

### Manual Linking

If not using CMake:

```bash
gcc main.c -o my_app \
    -I/usr/local/include \
    -L/usr/local/lib \
    -lzui -lwayland-client -lEGL -lGLESv2 -lfreetype -lxkbcommon -lm
```

## Verifying Installation

Create a simple test file `test.c`:

```c
#include <zui/zui.h>
#include <stdio.h>

int main(void)
{
    if (!zui_init()) {
        fprintf(stderr, "Failed to initialize ZUI\n");
        return 1;
    }
    
    ZuiWindow *window = zui_window_create(400, 300, "ZUI Test");
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }
    
    printf("ZUI initialized successfully!\n");
    
    zui_window_show(window);
    
    // Run for a moment
    for (int i = 0; i < 60; i++) {
        zui_window_render(window);
        zui_poll_events();
    }
    
    zui_window_destroy(window);
    zui_shutdown();
    
    printf("ZUI test passed!\n");
    return 0;
}
```

Compile and run:

```bash
gcc test.c -o test $(pkg-config --cflags --libs zui)
./test
```

## Troubleshooting

### "Cannot connect to Wayland display"

Make sure you're running under a Wayland compositor (not X11):

```bash
echo $XDG_SESSION_TYPE  # Should output "wayland"
```

### "EGL initialization failed"

Ensure your graphics drivers support OpenGL ES or OpenGL:

```bash
glxinfo | grep "OpenGL version"
```

### "Package zui not found"

If you installed to a custom prefix, set `PKG_CONFIG_PATH`:

```bash
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
```

---

**Next:** [Quick Start →](quick-start.md)
