#include <zui/resource.h>
#include <string.h>

/* Include ZUI internal resources (icons, etc.) */
#include "zui_internal_resources.h"

bool zui_is_resource_path(const char *path)
{
    if (!path) return false;
    return (strncmp(path, "res:", 4) == 0);
}

/* Weak default implementations - overridden by application's generated resources */
__attribute__((weak))
const unsigned char *zui_app_resource_get(const char *path, size_t *size)
{
    (void)path;
    if (size) *size = 0;
    return NULL;
}

__attribute__((weak))
bool zui_app_resource_exists(const char *path)
{
    (void)path;
    return false;
}

const unsigned char *zui_resource_get(const char *path, size_t *size)
{
    /* First try application resources */
    const unsigned char *data = zui_app_resource_get(path, size);
    if (data) return data;

    /* Fall back to internal library resources */
    return zui_internal_resource_get(path, size);
}

bool zui_resource_exists(const char *path)
{
    /* Check application resources first */
    if (zui_app_resource_exists(path)) return true;

    /* Check internal library resources */
    return zui_internal_resource_exists(path);
}
