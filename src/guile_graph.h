#ifndef GUPL_GRAPH_H
#define GUPL_GRAPH_H

#include <libguile.h>
#include "visibility.h"

/* Initialization */
void gupl_graph_init (void) __attribute__ ((visibility("default"))) ;

/* Functions */
GUPL_API SCM gupl_is_graph_p (SCM x);
GUPL_API SCM gupl_new_graph (void);
GUPL_API SCM gupl_save_screen_x (SCM s_graph);
GUPL_API SCM gupl_toggle_transpose_axes_x (SCM s_graph);
GUPL_API SCM gupl_toggle_auto_bump_x (SCM s_graph);
GUPL_API SCM gupl_toggle_use_color_x (SCM s_graph);
GUPL_API SCM gupl_toggle_frame_on_top_x (SCM s_graph);
GUPL_API SCM gupl_portable_output_x (SCM s_graph);
GUPL_API SCM gupl_emulate_color_x (SCM s_graph, SCM s_str);
GUPL_API SCM gupl_graph_version (void);
GUPL_API SCM gupl_toggle_rotate_y_label_x (SCM s_graph);
GUPL_API SCM gupl_font_size_x (SCM s_graph, SCM s_font);
GUPL_API SCM gupl_grid_style_x (SCM s_graph, SCM s_grid_style);
GUPL_API SCM gupl_height_of_plot_x (SCM s_graph, SCM s_height);
GUPL_API SCM gupl_clip_mode_x (SCM s_graph, SCM s_mode);
GUPL_API SCM gupl_toggle_log_axis_x (SCM s_graph, SCM s_arg);
GUPL_API SCM gupl_toggle_no_ticks_x (SCM s_graph, SCM s_arg);
GUPL_API SCM gupl_line_mode_x (SCM s_graph, SCM s_index);
GUPL_API SCM gupl_fill_fraction_x (SCM s_graph, SCM s_fraction);
GUPL_API SCM gupl_right_shift_x (SCM s_graph, SCM s_shift);
GUPL_API SCM gupl_upward_shift_x (SCM s_graph, SCM s_shift);
GUPL_API SCM gupl_width_of_plot_x (SCM s_graph, SCM s_width);
GUPL_API SCM gupl_output_format_x (SCM s_graph, SCM s_format);
GUPL_API SCM gupl_font_name_x (SCM s_graph, SCM s_name);
GUPL_API SCM gupl_rotation_angle_x (SCM s_graph, SCM s_angle);
GUPL_API SCM gupl_title_font_name_x (SCM s_graph, SCM s_name);
GUPL_API SCM gupl_symbol_font_name_x (SCM s_graph, SCM s_name);
GUPL_API SCM gupl_toggle_round_to_next_tick_x (SCM s_graph, SCM s_arg);
GUPL_API SCM gupl_top_label_x (SCM s_graph, SCM s_label);
GUPL_API SCM gupl_tick_size_x (SCM s_graph, SCM s_val);
GUPL_API SCM gupl_line_width_x (SCM s_graph, SCM s_val);
GUPL_API SCM gupl_x_label_x (SCM s_graph, SCM s_label);
GUPL_API SCM gupl_y_label_x (SCM s_graph, SCM s_label);
GUPL_API SCM gupl_toggle_axis_end (SCM s_graph, SCM s_arg);
GUPL_API SCM gupl_blankout_x (SCM s_graph, SCM s_val);
GUPL_API SCM gupl_bitmap_size_x (SCM s_graph, SCM s_val);
GUPL_API SCM gupl_title_font_size_x (SCM s_graph, SCM s_font);
GUPL_API SCM gupl_frame_line_width_x (SCM s_graph, SCM s_val);
GUPL_API SCM gupl_max_line_length_x (SCM s_graph, SCM s_val);
GUPL_API SCM gupl_page_size_x (SCM s_graph, SCM s_size);
GUPL_API SCM gupl_pen_colors (SCM s_colors);
GUPL_API SCM gupl_bg_color_x (SCM s_graph, SCM s_color);
GUPL_API SCM gupl_frame_color_x (SCM s_graph, SCM s_color);
GUPL_API SCM gupl_auto_abscissa_x (SCM s_graph, SCM s_delta_x, SCM s_x_start);
GUPL_API SCM gupl_x_limits_x (SCM s_graph, SCM s_min, SCM s_max, SCM s_spacing);
GUPL_API SCM gupl_y_limits_x (SCM s_graph, SCM s_min, SCM s_max, SCM s_spacing);
GUPL_API SCM gupl_symbol_x (SCM s_graph, SCM s_index, SCM s_size);
GUPL_API SCM gupl_reposition_x (SCM s_graph, SCM s_trans_x, SCM s_trans_y, SCM s_scale);
GUPL_API SCM gupl_xy_data_x (SCM s_graph, SCM s_xvec, SCM s_yvec);
GUPL_API SCM gupl_generate (SCM s_graph, SCM s_outp, SCM s_errp);
GUPL_API SCM gupl_graph_test (SCM s_graph, SCM s_name);
#endif
