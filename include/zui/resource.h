#ifndef ZUI_RESOURCE_H
#define ZUI_RESOURCE_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Get embedded resource data by path.
 *
 * Paths can be:
 *   - "res:/images/logo.png"
 *   - "res:images/logo.png"
 *   - "images/logo.png"
 *
 * @param path Resource path
 * @param size Output parameter for data size (can be NULL)
 * @return Pointer to resource data, or NULL if not found
 */
const unsigned char *zui_resource_get(const char *path, size_t *size);

/**
 * Check if a resource exists.
 *
 * @param path Resource path
 * @return true if resource exists
 */
bool zui_resource_exists(const char *path);

/**
 * Check if a path is a resource path (starts with "res:").
 *
 * @param path Path to check
 * @return true if path starts with "res:" or "res:/"
 */
bool zui_is_resource_path(const char *path);

#endif
