#ifndef ZUI_TYPES_H
#define ZUI_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct ZuiWidget ZuiWidget;
typedef struct ZuiWindow ZuiWindow;
typedef struct ZuiTitlebar ZuiTitlebar;
typedef struct ZuiButton ZuiButton;
typedef struct ZuiLabel ZuiLabel;
typedef struct ZuiContainer ZuiContainer;
typedef struct ZuiPanel ZuiPanel;
typedef struct ZuiCheckbox ZuiCheckbox;
typedef struct ZuiScrollView ZuiScrollView;
typedef struct ZuiScroller ZuiScroller;
typedef struct ZuiSplitView ZuiSplitView;
typedef struct ZuiRadioGroup ZuiRadioGroup;
typedef struct ZuiRadioButton ZuiRadioButton;
typedef struct ZuiTextInput ZuiTextInput;

#include <zui/color.h>
#include <zui/layout.h>

typedef struct ZuiPoint {
  float x, y;
} ZuiPoint;

typedef struct ZuiSize {
  float width, height;
} ZuiSize;

typedef struct ZuiBounds {
  float x, y, width, height;
} ZuiBounds;

typedef struct ZuiIcon ZuiIcon;

typedef enum ZuiWidgetType {
  ZUI_WIDGET_CONTAINER,
  ZUI_WIDGET_WINDOW,
  ZUI_WIDGET_TITLEBAR,
  ZUI_WIDGET_BUTTON,
  ZUI_WIDGET_LABEL,
  ZUI_WIDGET_ICON,
  ZUI_WIDGET_IMAGE,
  ZUI_WIDGET_PANEL,
  ZUI_WIDGET_CHECKBOX,
  ZUI_WIDGET_SCROLLVIEW,
  ZUI_WIDGET_SCROLLER,
  ZUI_WIDGET_SPLITVIEW,
  ZUI_WIDGET_RADIOBUTTON,
  ZUI_WIDGET_TEXTINPUT,
} ZuiWidgetType;

typedef enum ZuiSplitAnchor {
  ZUI_SPLIT_ANCHOR_AUTO,
  ZUI_SPLIT_ANCHOR_START,
  ZUI_SPLIT_ANCHOR_CENTER,
  ZUI_SPLIT_ANCHOR_END,
} ZuiSplitAnchor;

typedef void (*ZuiClickCallback)(ZuiWidget *widget, void *user_data);
typedef void (*ZuiDrawCallback)(ZuiWidget *widget, void *renderer);
typedef void (*ZuiCheckboxCallback)(ZuiCheckbox *checkbox, bool checked,
                                     void *user_data);
typedef void (*ZuiScrollerCallback)(ZuiScroller *scroller, float value,
                                     void *user_data);
typedef void (*ZuiRadioCallback)(ZuiRadioGroup *group, ZuiRadioButton *selected,
                                  void *user_data);
typedef void (*ZuiTextInputCallback)(ZuiTextInput *input, const char *text,
                                      void *user_data);

#endif
