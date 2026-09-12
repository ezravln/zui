# Code Formatting Guidelines

This document describes the code style conventions for ZUI.

## Language Standard

Use **C11**.

## Naming Conventions

| Element | Style | Example |
|---------|-------|---------|
| Functions | `snake_case` | `zui_window_create` |
| Variables | `snake_case` | `window_width` |
| Constants | `UPPER_SNAKE_CASE` | `ZUI_MAX_WINDOWS` |
| Macros | `UPPER_SNAKE_CASE` | `ZUI_ASSERT` |
| Types (structs, enums) | `PascalCase` | `ZuiWindow`, `ZuiEvent` |
| Enum values | `UPPER_SNAKE_CASE` | `ZUI_EVENT_KEY_PRESS` |

## Public API Prefix

All public API symbols must be prefixed with `zui_` or `Zui`:

```c
ZuiWindow *zui_window_create(int width, int height, const char *title);
void zui_window_destroy(ZuiWindow *window);
```

## Indentation

- Use **4 spaces** for indentation.
- Do not use tabs.

## Braces

Use K&R style braces:

```c
if (condition) {
    do_something();
} else {
    do_other();
}

void function_name(void)
{
    // function body
}
```

Functions place the opening brace on a new line. Control structures place it on the same line.

## Line Length

Keep lines under **100 characters** when practical.

## Pointer Declaration

Attach the `*` to the variable name:

```c
int *ptr;
ZuiWindow *window;
```

## Function Declarations

For long parameter lists, break after the opening parenthesis:

```c
ZuiResult zui_window_create_with_options(
    int width,
    int height,
    const char *title,
    ZuiWindowOptions *options)
{
    // ...
}
```

## Comments

- Prefer `//` for single-line comments.
- Use `/* */` for multi-line comments.
- Comments explain **why**, not what.
- Do not write obvious comments.

```c
// Good: explains why
// Skip processing if window is minimized to save resources
if (window->minimized) {
    return;
}

// Bad: states the obvious
// Check if window is minimized
if (window->minimized) {
    return;
}
```

## Header Guards

Use traditional include guards:

```c
#ifndef ZUI_MODULENAME_H
#define ZUI_MODULENAME_H

// header content

#endif
```

## Include Order

1. Corresponding header (for `.c` files).
2. Project headers.
3. System headers.

Separate groups with a blank line:

```c
#include "window.h"

#include "internal/renderer.h"
#include "internal/platform.h"

#include <stdlib.h>
#include <string.h>
```

## Struct Definitions

```c
typedef struct ZuiWindow {
    int width;
    int height;
    const char *title;
    bool running;
} ZuiWindow;
```

## Enum Definitions

```c
typedef enum ZuiEventType {
    ZUI_EVENT_NONE = 0,
    ZUI_EVENT_KEY_PRESS,
    ZUI_EVENT_KEY_RELEASE,
    ZUI_EVENT_POINTER_MOVE,
    ZUI_EVENT_POINTER_BUTTON,
} ZuiEventType;
```

Include trailing comma for easier diffs.

## Error Handling

Return explicit error codes:

```c
typedef enum ZuiResult {
    ZUI_SUCCESS = 0,
    ZUI_ERROR_INVALID_ARGUMENT,
    ZUI_ERROR_OUT_OF_MEMORY,
    ZUI_ERROR_PLATFORM,
} ZuiResult;

ZuiResult zui_init(void);
```

## Memory Management

- Pair every allocation with a clear deallocation path.
- Document ownership in function comments when not obvious.
- Avoid hidden allocations in public API functions.

## Example

```c
#pragma once

#include <stdbool.h>

typedef struct ZuiWindow ZuiWindow;

typedef enum ZuiResult {
    ZUI_SUCCESS = 0,
    ZUI_ERROR_INIT_FAILED,
} ZuiResult;

ZuiWindow *zui_window_create(int width, int height, const char *title);
void zui_window_destroy(ZuiWindow *window);
bool zui_window_running(ZuiWindow *window);
void zui_window_show(ZuiWindow *window);
```

```c
#include "zui/window.h"

#include "internal/platform.h"

#include <stdlib.h>

struct ZuiWindow {
    int width;
    int height;
    char *title;
    bool running;
    PlatformWindow *platform;
};

ZuiWindow *zui_window_create(int width, int height, const char *title)
{
    ZuiWindow *window = calloc(1, sizeof(ZuiWindow));
    if (!window) {
        return NULL;
    }

    window->width = width;
    window->height = height;
    window->running = true;

    window->platform = platform_window_create(width, height, title);
    if (!window->platform) {
        free(window);
        return NULL;
    }

    return window;
}

void zui_window_destroy(ZuiWindow *window)
{
    if (!window) {
        return;
    }

    platform_window_destroy(window->platform);
    free(window);
}
```
