#include <zui/zui.h>
#include <stdio.h>

static void on_button_click(ZuiWidget *widget, void *user_data)
{
  (void)widget;
  const char *name = user_data;
  printf("%s clicked!\n", name);
}

static void on_checkbox_change(ZuiCheckbox *checkbox, bool checked, void *user_data)
{
  (void)checkbox;
  const char *name = user_data;
  printf("%s: %s\n", name, checked ? "checked" : "unchecked");
}

static void on_radio_change(ZuiRadioGroup *group, ZuiRadioButton *selected, void *user_data)
{
  (void)group;
  (void)user_data;
  printf("Selected: %s\n", zui_radiobutton_get_label(selected));
}

static void on_text_change(ZuiTextInput *input, const char *text, void *user_data)
{
  (void)input;
  (void)user_data;
  printf("Text: %s\n", text);
}

static ZuiCircularProgress *g_cprogress = NULL;

static void on_slider_change(ZuiSlider *slider, float value, void *user_data)
{
  (void)slider;
  ZuiProgressBar *progress = user_data;
  if (progress) {
    zui_progressbar_set_value(progress, value / 100.0f);
  }
  if (g_cprogress) {
    zui_circularprogress_set_value(g_cprogress, value / 100.0f);
  }
  printf("Slider: %.1f\n", value);
}

static void on_piechart_hover(ZuiPieChart *chart, int slice_index, void *user_data)
{
  (void)chart;
  (void)user_data;
  if (slice_index >= 0) {
    printf("PieChart: hovering slice %d\n", slice_index);
  }
}

static void on_barchart_hover(ZuiBarChart *chart, int bar_index, void *user_data)
{
  (void)chart;
  (void)user_data;
  if (bar_index >= 0) {
    printf("BarChart: hovering bar %d\n", bar_index);
  }
}

static void on_dropdown_change(ZuiDropdown *dropdown, int index, const char *item, void *user_data)
{
  (void)dropdown;
  (void)user_data;
  printf("Dropdown selected [%d]: %s\n", index, item);
}

static void on_menu_item_click(ZuiMenuItem *item, void *user_data)
{
  (void)item;
  const char *action = (const char *)user_data;
  printf("Menu clicked: %s\n", action ? action : "unknown");
}

static void on_video_end(ZuiVideo *video, void *user_data)
{
  (void)video;
  (void)user_data;
  printf("Video ended!\n");
}

static ZuiVideo *g_video = NULL;

static void on_video_toggle(ZuiWidget *widget, void *user_data)
{
  (void)widget;
  (void)user_data;
  if (g_video) {
    zui_video_toggle_playback(g_video);
  }
}

static ZuiScrollView *g_scrollview = NULL;
static ZuiScroller *g_scroller = NULL;

static void on_scroller_change(ZuiScroller *scroller, float value, void *user_data)
{
  (void)scroller;
  (void)user_data;
  if (g_scrollview) {
    zui_scrollview_scroll_to(g_scrollview, 0, value);
  }
}

static void on_scrollview_scroll(ZuiScrollView *sv, float x, float y, void *user_data)
{
  (void)sv;
  (void)x;
  (void)user_data;
  if (g_scroller) {
    zui_scroller_set_value(g_scroller, y);
  }
}

int main(void)
{
  if (!zui_init()) {
    fprintf(stderr, "Failed to initialize ZUI\n");
    return 1;
  }

  ZuiWindow *window = zui_window_create(900, 600, "ZUI Panel Demo");
  if (!window) {
    fprintf(stderr, "Failed to create window\n");
    zui_shutdown();
    return 1;
  }

  zui_window_set_corner_radius(window, 12.0f);

  ZuiTitlebar *titlebar = zui_window_titlebar(window);

  ZuiLabel *title = zui_label_create("Panel Layout Demo");
  zui_titlebar_set_center(titlebar, (ZuiWidget *)title);

  zui_titlebar_set_end(titlebar, zui_window_hide_button(window));
  zui_titlebar_set_end(titlebar, zui_window_maximize_button(window));
  zui_titlebar_set_end(titlebar, zui_window_close_button(window));

  ZuiWidget *content = zui_window_content(window);

  ZuiPanel *main_panel = zui_panel_create();
  zui_panel_set_padding(main_panel, 5.0, 5.0, 5.0, 5.0);
  zui_panel_set_fill(main_panel, true, true);

  ZuiSplitView *splitview = zui_splitview_create(false);
  zui_set_fill(zui_splitview_as_widget(splitview), true, true);
  zui_splitview_set_min_child_size(splitview, 150.0f);
  zui_panel_add_child(main_panel, zui_splitview_as_widget(splitview));
  zui_widget_add_child(content, zui_panel_as_widget(main_panel));

  ZuiPanel *sidebar = zui_panel_create();
  zui_panel_set_layout(sidebar, ZUI_LAYOUT_VERTICAL);
  zui_set_fill(zui_panel_as_widget(sidebar), true, true);
  zui_set_background(zui_panel_as_widget(sidebar), ZUI_COLOR_HEX(0x2d2d2d));
  zui_set_corner_radius(zui_panel_as_widget(sidebar), 8.0f);
  zui_panel_set_padding(sidebar, 15.0f, 15.0f, 15.0f, 15.0f);
  zui_set_spacing(zui_panel_as_widget(sidebar), 8.0f);
  zui_splitview_add_child(splitview, zui_panel_as_widget(sidebar));

  ZuiLabel *sidebar_title = zui_label_create("Navigation");
  zui_label_set_size(sidebar_title, 18.0f);
  zui_label_set_color(sidebar_title, ZUI_COLOR_HEX(0xcccccc));
  zui_panel_add_child(sidebar, (ZuiWidget *)sidebar_title);
  zui_panel_set_spacing(sidebar, 4.0f);

  const char *nav_items[] = {"Dashboard", "Settings", "Profile", "Help"};
  for (int i = 0; i < 4; i++) {
    ZuiButton *btn = zui_button_create(nav_items[i]);
    zui_button_set_size(btn, 170.0f, 36.0f);
    zui_set_fill((ZuiWidget*)btn, true, false);
    zui_button_set_color(btn, ZUI_COLOR_HEX(0x3d3d3d));
    zui_button_on_click(btn, on_button_click, (void *)nav_items[i]);
    zui_panel_add_child(sidebar, (ZuiWidget *)btn);
  }

  ZuiPanel *content_outer = zui_panel_create();
  zui_panel_set_layout(content_outer, ZUI_LAYOUT_VERTICAL);
  zui_set_fill(zui_panel_as_widget(content_outer), true, true);
  zui_set_background(zui_panel_as_widget(content_outer), ZUI_COLOR_HEX(0x2d2d2d));
  zui_set_corner_radius(zui_panel_as_widget(content_outer), 8.0f);
  zui_splitview_add_child(splitview, zui_panel_as_widget(content_outer));
  zui_splitview_set_handle_position(splitview, 0, 200.0f);

  ZuiMenuBar *menubar = zui_menubar_create();
  zui_menubar_set_size(menubar, 0, 28.0f);
  zui_set_fill(zui_menubar_as_widget(menubar), true, false);

  ZuiMenu *file_menu = zui_menu_create("File");
  ZuiMenuItem *new_item = zui_menuitem_create("New");
  zui_menuitem_set_icon(new_item, "assets/icons/x-file-icon.svg", 14.0f);
  zui_menuitem_set_shortcut(new_item, "Ctrl+N");
  zui_menuitem_on_click(new_item, on_menu_item_click, "New");
  zui_menu_add_item(file_menu, new_item);

  ZuiMenuItem *open_item = zui_menuitem_create("Open");
  zui_menuitem_set_icon(open_item, "assets/icons/x-file-icon.svg", 14.0f);
  zui_menuitem_set_shortcut(open_item, "Ctrl+O");
  zui_menuitem_on_click(open_item, on_menu_item_click, "Open");
  zui_menu_add_item(file_menu, open_item);

  ZuiMenuItem *save_item = zui_menuitem_create("Save");
  zui_menuitem_set_shortcut(save_item, "Ctrl+S");
  zui_menuitem_on_click(save_item, on_menu_item_click, "Save");
  zui_menu_add_item(file_menu, save_item);

  zui_menu_add_separator(file_menu);

  ZuiMenuItem *exit_item = zui_menuitem_create("Exit");
  zui_menuitem_set_shortcut(exit_item, "Alt+F4");
  zui_menuitem_on_click(exit_item, on_menu_item_click, "Exit");
  zui_menu_add_item(file_menu, exit_item);
  zui_menubar_add_menu(menubar, file_menu);

  ZuiMenu *edit_menu = zui_menu_create("Edit");
  ZuiMenuItem *undo_item = zui_menuitem_create("Undo");
  zui_menuitem_set_shortcut(undo_item, "Ctrl+Z");
  zui_menuitem_on_click(undo_item, on_menu_item_click, "Undo");
  zui_menu_add_item(edit_menu, undo_item);

  ZuiMenuItem *redo_item = zui_menuitem_create("Redo");
  zui_menuitem_set_shortcut(redo_item, "Ctrl+Y");
  zui_menuitem_on_click(redo_item, on_menu_item_click, "Redo");
  zui_menu_add_item(edit_menu, redo_item);

  zui_menu_add_separator(edit_menu);

  ZuiMenuItem *cut_item = zui_menuitem_create("Cut");
  zui_menuitem_set_shortcut(cut_item, "Ctrl+X");
  zui_menu_add_item(edit_menu, cut_item);

  ZuiMenuItem *copy_item = zui_menuitem_create("Copy");
  zui_menuitem_set_shortcut(copy_item, "Ctrl+C");
  zui_menu_add_item(edit_menu, copy_item);

  ZuiMenuItem *paste_item = zui_menuitem_create("Paste");
  zui_menuitem_set_shortcut(paste_item, "Ctrl+V");
  zui_menu_add_item(edit_menu, paste_item);
  zui_menubar_add_menu(menubar, edit_menu);

  ZuiMenu *help_menu = zui_menu_create("Help");
  ZuiMenuItem *about_item = zui_menuitem_create("About");
  zui_menuitem_on_click(about_item, on_menu_item_click, "About");
  zui_menu_add_item(help_menu, about_item);
  zui_menubar_add_menu(menubar, help_menu);

  zui_panel_add_child(content_outer, zui_menubar_as_widget(menubar));

  ZuiScrollView *content_scroll = zui_scrollview_create();
  zui_set_fill(zui_scrollview_as_widget(content_scroll), true, true);
  zui_panel_add_child(content_outer, zui_scrollview_as_widget(content_scroll));

  ZuiPanel *content_panel = zui_panel_create();
  zui_panel_set_layout(content_panel, ZUI_LAYOUT_VERTICAL);
  zui_panel_set_padding(content_panel, 20.0f, 20.0f, 20.0f, 20.0f);
  zui_set_spacing(zui_panel_as_widget(content_panel), 15.0f);
  zui_set_size(zui_panel_as_widget(content_panel), 400.0f, 950.0f);
  zui_scrollview_set_content(content_scroll, zui_panel_as_widget(content_panel));

  ZuiLabel *content_title = zui_label_create("Welcome to ZUI");
  zui_label_set_size(content_title, 28.0f);
  zui_label_set_color(content_title, ZUI_COLOR_HEX(0xffffff));
  zui_panel_add_child(content_panel, (ZuiWidget *)content_title);

  ZuiLabel *description = zui_label_create("A lightweight modern GUI toolkit");
  zui_label_set_size(description, 16.0f);
  zui_label_set_color(description, ZUI_COLOR_HEX(0x888888));
  zui_panel_add_child(content_panel, (ZuiWidget *)description);

  ZuiCheckbox *checkbox1 = zui_checkbox_create("Enable notifications");
  zui_checkbox_set_box_color(checkbox1, ZUI_COLOR_HEX(0x3d3d3d), ZUI_COLOR_HEX(0x3d3d3d));
  zui_checkbox_set_icon_color(checkbox1, ZUI_COLOR_HEX(0xffffff));
  zui_checkbox_on_change(checkbox1, on_checkbox_change, "Notifications");
  zui_panel_add_child(content_panel, (ZuiWidget *)checkbox1);

  ZuiCheckbox *checkbox2 = zui_checkbox_create("Dark mode");
  zui_checkbox_set_box_color(checkbox2, ZUI_COLOR_HEX(0x3d3d3d), ZUI_COLOR_HEX(0x22c55e));
  zui_checkbox_set_icon_color(checkbox2, ZUI_COLOR_HEX(0xffffff));
  zui_checkbox_set_checked(checkbox2, true);
  zui_checkbox_on_change(checkbox2, on_checkbox_change, "Dark mode");
  zui_panel_add_child(content_panel, (ZuiWidget *)checkbox2);

  ZuiTextInput *text_input = zui_textinput_create("Enter your name...");
  zui_textinput_set_size(text_input, 280.0f, 36.0f);
  zui_textinput_on_change(text_input, on_text_change, NULL);
  zui_panel_add_child(content_panel, zui_textinput_as_widget(text_input));

  ZuiTextInput *text_input2 = zui_textinput_create("Enter email...");
  zui_textinput_set_size(text_input2, 280.0f, 36.0f);
  zui_panel_add_child(content_panel, zui_textinput_as_widget(text_input2));

  ZuiTextInput *text_input3 = zui_textinput_create("Enter message...");
  zui_textinput_set_size(text_input3, 280.0f, 36.0f);
  zui_panel_add_child(content_panel, zui_textinput_as_widget(text_input3));

  ZuiProgressBar *progress = zui_progressbar_create();
  zui_progressbar_set_size(progress, 280.0f, 10.0f);
  zui_progressbar_set_value(progress, 0.5f);
  zui_panel_add_child(content_panel, zui_progressbar_as_widget(progress));

  ZuiSlider *slider = zui_slider_create(0.0f, 100.0f, 50.0f);
  zui_slider_set_size(slider, 280.0f, 24.0f);
  zui_slider_on_change(slider, on_slider_change, progress);
  zui_panel_add_child(content_panel, zui_slider_as_widget(slider));

  ZuiDropdown *dropdown = zui_dropdown_create("Select an option...");
  zui_dropdown_set_size(dropdown, 200.0f, 36.0f);
  zui_dropdown_add_item(dropdown, "Option 1");
  zui_dropdown_add_item(dropdown, "Option 2");
  zui_dropdown_add_item(dropdown, "Option 3");
  zui_dropdown_add_item(dropdown, "Custom Option");
  zui_dropdown_on_change(dropdown, on_dropdown_change, NULL);
  zui_panel_add_child(content_panel, zui_dropdown_as_widget(dropdown));

  ZuiGridView *gridview = zui_gridview_create(4);
  zui_gridview_set_size(gridview, 300.0f, 150.0f);
  zui_gridview_set_gap(gridview, 6.0f, 6.0f);
  zui_gridview_set_padding(gridview, 6.0f);
  zui_gridview_set_background(gridview, ZUI_COLOR_HEX(0x2a2a2a));
  for (int i = 0; i < 20; i++) {
    ZuiButton *grid_btn = zui_button_create("");
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", i + 1);
    zui_button_set_text(grid_btn, buf);
    zui_button_set_size(grid_btn, 60.0f, 60.0f);
    zui_gridview_add_child(gridview, (ZuiWidget *)grid_btn);
  }
  zui_panel_add_child(content_panel, zui_gridview_as_widget(gridview));

  ZuiPanel *charts_panel = zui_panel_create();
  zui_panel_set_layout(charts_panel, ZUI_LAYOUT_HORIZONTAL);
  zui_set_spacing(zui_panel_as_widget(charts_panel), 15.0f);
  zui_set_size(zui_panel_as_widget(charts_panel), 300.0f, 100.0f);
  zui_panel_add_child(content_panel, zui_panel_as_widget(charts_panel));

  ZuiPieChart *piechart = zui_piechart_create();
  zui_piechart_set_size(piechart, 80.0f);
  zui_piechart_add_slice_labeled(piechart, 30, ZUI_COLOR_HEX(0x4a9eff), "A");
  zui_piechart_add_slice_labeled(piechart, 25, ZUI_COLOR_HEX(0x22c55e), "B");
  zui_piechart_add_slice_labeled(piechart, 20, ZUI_COLOR_HEX(0xf59e0b), "C");
  zui_piechart_add_slice_labeled(piechart, 25, ZUI_COLOR_HEX(0xef4444), "D");
  zui_piechart_set_show_values(piechart, true);
  zui_piechart_on_hover(piechart, on_piechart_hover, NULL);
  zui_panel_add_child(charts_panel, zui_piechart_as_widget(piechart));

  ZuiBarChart *barchart = zui_barchart_create();
  zui_barchart_set_size(barchart, 120.0f, 80.0f);
  zui_barchart_add_bar_labeled(barchart, 70, ZUI_COLOR_HEX(0x4a9eff), "Mon");
  zui_barchart_add_bar_labeled(barchart, 50, ZUI_COLOR_HEX(0x22c55e), "Tue");
  zui_barchart_add_bar_labeled(barchart, 90, ZUI_COLOR_HEX(0xf59e0b), "Wed");
  zui_barchart_add_bar_labeled(barchart, 40, ZUI_COLOR_HEX(0xef4444), "Thu");
  zui_barchart_add_bar_labeled(barchart, 60, ZUI_COLOR_HEX(0xa855f7), "Fri");
  zui_barchart_set_show_values(barchart, true);
  zui_barchart_on_hover(barchart, on_barchart_hover, NULL);
  zui_panel_add_child(charts_panel, zui_barchart_as_widget(barchart));

  ZuiCircularProgress *cprogress = zui_circularprogress_create();
  zui_circularprogress_set_size(cprogress, 80.0f);
  zui_circularprogress_set_value(cprogress, 0.65f);
  zui_circularprogress_set_thickness(cprogress, 8.0f);
  zui_circularprogress_set_colors(cprogress, ZUI_COLOR_HEX(0x333333),
                                   ZUI_COLOR_HEX(0x22c55e));
  zui_circularprogress_set_show_percentage(cprogress, true);
  g_cprogress = cprogress;
  zui_panel_add_child(charts_panel, zui_circularprogress_as_widget(cprogress));

  ZuiLabel *video_label = zui_label_create("Video Player");
  zui_label_set_size(video_label, 14.0f);
  zui_label_set_color(video_label, ZUI_COLOR_HEX(0x888888));
  zui_panel_add_child(content_panel, (ZuiWidget *)video_label);

  ZuiVideo *video = zui_video_create(NULL);
  if (video) {
    g_video = video;
    zui_video_set_size(video, 320.0f, 180.0f);
    zui_video_set_loop(video, false);
    zui_video_on_end(video, on_video_end, NULL);
    zui_video_open(video, "assets/video/video.mp4");
    zui_video_play(video);
    ZuiWidget *video_widget = zui_video_as_widget(video);
    zui_set_corner_radius(video_widget, 8.0f);
    zui_panel_add_child(content_panel, video_widget);

    ZuiButton *video_toggle = zui_button_create("Play/Pause");
    zui_button_set_size(video_toggle, 100.0f, 30.0f);
    zui_button_on_click(video_toggle, on_video_toggle, NULL);
    zui_panel_add_child(content_panel, (ZuiWidget *)video_toggle);
  }

  ZuiRadioGroup *radio_group = zui_radiogroup_create();
  zui_radiogroup_on_change(radio_group, on_radio_change, NULL);

  ZuiRadioButton *radio1 = zui_radiobutton_create(radio_group, "Option A");
  zui_panel_add_child(content_panel, zui_radiobutton_as_widget(radio1));

  ZuiRadioButton *radio2 = zui_radiobutton_create(radio_group, "Option B");
  zui_panel_add_child(content_panel, zui_radiobutton_as_widget(radio2));

  ZuiRadioButton *radio3 = zui_radiobutton_create(radio_group, "Option C");
  zui_radiobutton_set_colors(radio3, ZUI_COLOR_HEX(0x3d3d3d),
                              ZUI_COLOR_HEX(0x22c55e), ZUI_COLOR_HEX(0xffffff));
  zui_panel_add_child(content_panel, zui_radiobutton_as_widget(radio3));

  ZuiPanel *scroll_container = zui_panel_create();
  zui_panel_set_layout(scroll_container, ZUI_LAYOUT_HORIZONTAL);
  zui_set_size(zui_panel_as_widget(scroll_container), 320.0f, 150.0f);
  zui_set_spacing(zui_panel_as_widget(scroll_container), 4.0f);
  zui_panel_add_child(content_panel, zui_panel_as_widget(scroll_container));

  ZuiScrollView *scrollview = zui_scrollview_create();
  zui_set_size(zui_scrollview_as_widget(scrollview), 300.0f, 150.0f);
  zui_set_background(zui_scrollview_as_widget(scrollview), ZUI_COLOR_HEX(0x1a1a1a));
  zui_set_corner_radius(zui_scrollview_as_widget(scrollview), 8.0f);
  g_scrollview = scrollview;

  ZuiPanel *scroll_content = zui_panel_create();
  zui_panel_set_layout(scroll_content, ZUI_LAYOUT_VERTICAL);
  zui_set_size(zui_panel_as_widget(scroll_content), 280.0f, 400.0f);
  zui_panel_set_padding(scroll_content, 10.0f, 10.0f, 10.0f, 10.0f);
  zui_set_spacing(zui_panel_as_widget(scroll_content), 8.0f);

  for (int i = 0; i < 10; i++) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Scroll Item %d", i + 1);
    ZuiLabel *item = zui_label_create(buf);
    zui_label_set_color(item, ZUI_COLOR_HEX(0xaaaaaa));
    zui_panel_add_child(scroll_content, (ZuiWidget *)item);
  }

  zui_scrollview_set_content(scrollview, zui_panel_as_widget(scroll_content));
  zui_scrollview_on_scroll(scrollview, on_scrollview_scroll, NULL);
  zui_panel_add_child(scroll_container, zui_scrollview_as_widget(scrollview));

  ZuiScroller *scroller = zui_scroller_create(true);
  zui_set_size(zui_scroller_as_widget(scroller), 10.0f, 150.0f);
  zui_scroller_set_range(scroller, 400.0f, 150.0f);
  zui_scroller_on_change(scroller, on_scroller_change, NULL);
  g_scroller = scroller;
  zui_panel_add_child(scroll_container, zui_scroller_as_widget(scroller));

  ZuiPanel *button_row = zui_panel_create();
  zui_panel_set_layout(button_row, ZUI_LAYOUT_HORIZONTAL);
  zui_set_size(zui_panel_as_widget(button_row), 400.0f, 50.0f);
  zui_set_spacing(zui_panel_as_widget(button_row), 10.0f);
  zui_panel_add_child(content_panel, zui_panel_as_widget(button_row));

  ZuiButton *primary_btn = zui_button_create("Click Me");
  zui_set_size((ZuiWidget *)primary_btn, 140.0f, 40.0f);
  zui_button_set_color(primary_btn, ZUI_COLOR_HEX(0x6E6ADE));
  zui_button_set_icon(primary_btn, "assets/logo/zui-logo-white.png", 20.0f);
  zui_button_on_click(primary_btn, on_button_click, "Click Me");
  zui_panel_add_child(button_row, (ZuiWidget *)primary_btn);

  ZuiButton *secondary_btn = zui_button_create("Secondary");
  zui_set_size((ZuiWidget *)secondary_btn, 100.0f, 40.0f);
  zui_button_set_color(secondary_btn, ZUI_COLOR_HEX(0x4a4a4a));
  zui_button_on_click(secondary_btn, on_button_click, "Secondary");
  zui_panel_add_child(button_row, (ZuiWidget *)secondary_btn);

  ZuiButton *danger_btn = zui_button_create("Danger");
  zui_set_size((ZuiWidget *)danger_btn, 100.0f, 40.0f);
  zui_button_set_color(danger_btn, ZUI_COLOR_HEX(0xe81123));
  zui_button_on_click(danger_btn, on_button_click, "Danger");
  zui_panel_add_child(button_row, (ZuiWidget *)danger_btn);

  zui_window_show(window);

  while (zui_window_running(window)) {
    zui_poll_events();
    zui_window_render(window);
  }

  zui_window_destroy(window);
  zui_shutdown();

  return 0;
}
