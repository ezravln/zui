#ifndef ZUI_APP_INTERNAL_H
#define ZUI_APP_INTERNAL_H

#include <stdbool.h>

typedef struct ZuiPlatform ZuiPlatform;
typedef struct ZuiEglContext ZuiEglContext;
typedef struct ZuiRenderer ZuiRenderer;
typedef struct ZuiApp ZuiApp;

ZuiApp *__zui_get_app(void);
ZuiPlatform *__zui_get_platform(void);
ZuiEglContext *__zui_get_egl(void);
ZuiRenderer *__zui_get_renderer(void);
bool __zui_renderer_is_initialized(void);
bool __zui_init_renderer_if_needed(void);

#endif
