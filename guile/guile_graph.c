/* Copyright (C) 1989,
   1990, 1991, 1995, 1996, 1997, 1998, 1999, 2000, 2005, 2008, 2009, Free
   Software Foundation, Inc.

   Copyright (C) 2010, Michael L. Gran

   This file is based on a file that is part of the GNU plotutils package.

   The GNU plotutils package is free software.  You may redistribute it
   and/or modify it under the terms of the GNU General Public License as
   published by the Free Software foundation; either version 2, or (at your
   option) any later version.

   The GNU plotutils package is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with the GNU plotutils package; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin St., Fifth Floor,
   Boston, MA 02110-1301, USA. */

/* This file contains the guile interface and a few support subroutines, for
   GNU graph. */

#define _GNU_SOURCE
#include "sys-defines.h"
#include "extern.h"
#include "libcommon.h"
#include "getopt.h"
#include "fontlist.h"
#include <libguile.h>

const char *progname = "graph";	/* name of this program */
const char *written = "Written by Robert S. Maier.";
const char *copyright = "Copyright (C) 2009 Free Software Foundation, Inc.";

struct option long_options[] = {};

static scm_t_bits graph_tag;

typedef enum { IN_PROGRESS, ENDED_BY_EOF, ENDED_BY_DATASET_TERMINATOR, ENDED_BY_MODE_CHANGE } dataset_status_t;

struct VecReaderStruct		/* point reader datatype */
{
/* parameters which are constant over the lifetime of a Reader, and which
   affect the computation of the returned point structures */
  bool transpose_axes;		/* x <-> y ? */
  int log_axis;			/* x,y axes are logarithmic? (portmanteau) */
  SCM xvec;
  SCM yvec;
  SCM yerrvec;
  size_t i_vec;		 /* index of current vector element */
  data_type format_type;	/* stream format (T_ASCII, T_DOUBLE, etc.) */
  bool auto_abscissa;		/* auto-generate x values?  */
  double delta_x;		/* increment for x value, if auto-generated */
  double initial_abscissa;	/* initial value for x, if auto-generated */
  bool auto_bump;		/* bump linemode when starting next polyline?*/
/* Reader parameters that are constant for the duration of each dataset */
  int symbol;			/* symbol type */
  double symbol_size;		/* symbol size (in `box coordinates') */
  const char *symbol_font_name;	/* font used for marker types >= 32 */
  int linemode;			/* linemode */
  double line_width;		/* line width (as frac. of display size) */
  double fill_fraction;		/* in interval [0,1], <0 means no filling */
  bool use_color;		/* color/monochrome interp. of linemode */
/* state variables, updated during Reader operation */
  bool need_break;		/* draw next point with pen up ? */
  double abscissa;		/* x value, if auto-generated */
};

typedef struct VecReaderStruct VecReader;

struct graph {
  int option;
  int opt_index;
  int errcnt;		    /* errors encountered in getopt parsing */
  int matched;
  bool using_getopt;	/* true until end of command-line options */
  bool continue_parse;	/* reset e.g. when --help or --version seen */
  bool show_version;	/* show version message? */
  bool show_usage;	/* show usage message? */
  bool show_fonts;	/* supply help on fonts? */
  bool do_list_fonts;	/* show a list of fonts? */
  bool filter;		/* will we act as a filter? */
  bool new_symbol;
  bool new_symbol_size;
  bool new_symbol_font_name;
  bool new_linemode;
  bool new_plot_line_width;
  bool new_fill_fraction;
  bool new_use_color;
  bool first_file_of_graph;
  bool first_graph_of_multigraph;
  FILE *data_file;

  /* Variables related to the point reader */

  VecReader *reader;
  data_type input_type;	 /* by default we read ascii data */
  bool auto_bump;	 /* auto-bump linemode between polylines? */
  bool auto_abscissa;	 /* generate abscissa values automatically? */
  double x_start;	 /* start and increment, for auto-abscissa */
  double delta_x;
  /* polyline attributes */
  int linemode_index;	   /* linemode for polylines, 1=solid, etc. */
  double plot_line_width; /* polyline width (as frac. of display width),
			     negative means default provided by libplot) */
  int symbol_index;	/* 0=none, 1=dot, 2=plus, 3=asterisk, etc. */
  double symbol_size;	/* symbol size (frac. of plotting box size) */
  double fill_fraction;	/* negative means regions aren't filled */
  bool use_color;	/* color / monochrome */

  /* Variables related to both the point reader and the point plotter */

  bool transpose_axes;	   /* true means -x applies to y axis, etc. */

  /* Variables related to the multigrapher, i.e. point plotter */

  Multigrapher *multigrapher;

  /* command-line parameters (constant over multigrapher operation) */
  const char *output_format;	/* libplot output format */
  const char *bg_color;		/* color of background, if non-NULL */
  const char *bitmap_size;
  const char *emulate_color;
  const char *max_line_length;
  const char *meta_portable;
  const char *page_size;
  const char *rotation_angle;
  bool save_screen;	 /* save screen, i.e. no erase before plot? */

  /* graph-specific parameters (may change from graph to graph) */

  grid_type grid_spec;	     /* frame type for current graph */
  bool no_rotate_y_label;    /* used for pre-X11R6 servers */
  const char *frame_color;   /* color of frame (and graph, if no -C)*/
  int clip_mode;	     /* clipping mode (cf. gnuplot) */
  /* following variables are portmanteau: x and y are included as bitfields*/
  int log_axis;		      /* log axes or linear axes? */
  int round_to_next_tick;     /* round axis limits to nearest tick? */
  int switch_axis_end; /* axis at top/right instead of bottom/left? */
  int omit_ticks;      /* omit ticks and tick labels from an axis? */

  /* graph dimensions, expressed as fractions of the width of the libplot
     graphics display [by convention square]; <0.0 means use libplot default */
  double frame_line_width;     /* width of lines in the graph frame */

  /* dimensions of graphing area, expressed as fractions of the width of
     the libplot graphics display [by convention square] */
  double margin_below;		/* margin below the plot */
  double margin_left;		/* margin left of the plot */
  double plot_height;		/* height of the plot */
  double plot_width;		/* width of the plot */

  /* dimensions, expressed as fractions of the size of the plotting area */
  double tick_size;	      /* size of tick marks (< 0.0 allowed) */
  double font_size;	      /* fontsize */
  double title_font_size;     /* title fontsize */
  double blankout_fraction;   /* this fraction of size of plotting box
				 is erased before the plot is drawn */

  /* text-related */
  const char *font_name;       /* font name, NULL -> device default */
  const char *title_font_name; /* title font name, NULL -> default */
  const char *symbol_font_name; /* symbol font name, NULL -> default */
  const char *x_label;	  /* label for the x axis, NULL -> no label */
  const char *y_label;	  /* label for the y axis, NULL -> no label */
  const char *top_label;  /* title above the plot, NULL -> no title */

  /* user-specified limits on the axes */
  double min_x, min_y, max_x, max_y;
  double spacing_x, spacing_y;

  /* flags indicating which axis limits the user has specified */
  bool spec_min_x, spec_min_y;
  bool spec_max_x, spec_max_y;
  bool spec_spacing_x, spec_spacing_y;

  /* misc. local variables used in getopt parsing, counterparts to the above */
  double local_x_start, local_delta_x;
  int local_grid_style;
  int local_symbol_index;
  int local_clip_mode;
  double local_symbol_size, local_font_size, local_title_font_size;
  double local_frame_line_width, local_plot_line_width;
  double local_min_x, local_min_y;
  double local_max_x, local_max_y;
  double local_spacing_x, local_spacing_y;
  double local_fill_fraction;

  /* `finalized' arguments to set_graph_parameters() (computed at the time
     the first file of a graph is seen, and continuing in effect over the
     duration of the graph) */
  int final_log_axis;
  int final_round_to_next_tick;
  double final_min_x, final_max_x, final_spacing_x;
  double final_min_y, final_max_y, final_spacing_y;
  bool final_spec_min_x, final_spec_min_y;
  bool final_spec_max_x, final_spec_max_y;
  bool final_spec_spacing_x, final_spec_spacing_y;
  bool final_transpose_axes;

  /* for storage of data points (if we're not acting as a filter) */
  Point *p;		   /* points array */
  int points_length;	   /* length of the points array, in points */
  int no_of_points;	   /* number of points stored in it */

  /* support for multigraphing */
  double reposition_trans_x, reposition_trans_y;
  double reposition_scale;
  double old_reposition_trans_x, old_reposition_trans_y;
  double old_reposition_scale;

  /* sui generis */
  bool frame_on_top;

  bool need_break;
};

typedef struct graph graph_t;


/* forward references */
static void initialize_struct_graph (graph_t *p);
static int _scm_is_graph (SCM x);
static graph_t *_scm_to_graph (SCM x);
static SCM equalp_graph (SCM x1, SCM x2);
static SCM mark_graph (SCM x);
static size_t free_graph (SCM x);
static int print_graph (SCM x, SCM port, scm_print_state * pstate);
static bool parse_pen_string (const char *pen_s);
static dataset_status_t read_vector_dataset (VecReader *reader, Point **p_addr, int *length, int *no_of_points);
static dataset_status_t read_vector_point (VecReader *reader, Point *point);
static dataset_status_t read_point_from_list (VecReader *reader, Point *point);
void gupl_graph_init (void);
static void reset_vec_reader (VecReader *reader);

#define NEW_POINT_ARRAY_LENGTH(old_len)				\
  ((old_len)*sizeof(Point) < 10000000				\
   ? 2 * (old_len) : (old_len) + 10000000/sizeof(Point))


void
initialize_struct_graph (graph_t *p)
{
  p->errcnt = 0;
  p->using_getopt = true;
  p->continue_parse = true;
  p->show_version = false;
  p->show_usage = false;
  p->show_fonts = false;
  p->do_list_fonts = false;
  p->filter = false;
  p->new_symbol = false;
  p->new_symbol_size = false;
  p->new_symbol_font_name = false;
  p->new_linemode = false;
  p->new_plot_line_width = false;
  p->new_fill_fraction = false;
  p->new_use_color = false;
  p->first_file_of_graph = true;
  p->first_graph_of_multigraph = true;
  p->data_file = NULL;

  p->reader = NULL;
  p->input_type = T_ASCII;
  p->auto_bump = true;
  p->auto_abscissa = false;
  p->x_start = 0.;
  p->delta_x = 1.;

  p->linemode_index = 1;
  p->plot_line_width = -0.001;
  p->symbol_index = 0;
  p->symbol_size = .03;
  p->fill_fraction = -1.0;
  p->use_color = false;

  p->transpose_axes = false;

  p->multigrapher = NULL;

  p->output_format = "X"; /* by default, pop up X window */
  p->bg_color = NULL;
  p->bitmap_size = NULL;
  p->emulate_color = NULL;
  p->max_line_length = NULL;
  p->meta_portable = NULL;
  p->page_size = NULL;
  p->rotation_angle = NULL;
  p->save_screen = false;

  p->grid_spec = AXES_AND_BOX;
  p->no_rotate_y_label = false;
  p->frame_color = "black";
  p->clip_mode = 1;

  p->log_axis = 0;
  p->round_to_next_tick = 0;
  p->switch_axis_end = 0;
  p->omit_ticks = 0;

  p->frame_line_width = -0.001;

  p->margin_below = .2;
  p->margin_left = .2;
  p->plot_height = .6;
  p->plot_width = .6;

  p->tick_size = .02;
  p->font_size = 0.0525;
  p->title_font_size = 0.07;
  p->blankout_fraction = 1.3;

  p->font_name = NULL;
  p->title_font_name = NULL;
  p->symbol_font_name = "ZapfDingbats";
  p->x_label = NULL;
  p->y_label = NULL;
  p->top_label = NULL;

  p->min_x = 0.0;
  p->min_y = 0.0;
  p->max_x = 0.0;
  p->max_y = 0.0;
  p->spacing_x = 0.0;
  p->spacing_y = 0.0;

  p->spec_min_x = false;
  p->spec_min_y = false;
  p->spec_max_x = false;
  p->spec_max_y = false;
  p->spec_spacing_x = false;
  p->spec_spacing_y = false;

  p->final_log_axis = 0;
  p->final_round_to_next_tick = 0;
  p->final_min_x = 0.0;
  p->final_max_x = 0.0;
  p->final_spacing_x = 0.0;
  p->final_min_y = 0.0;
  p->final_max_y = 0.0;
  p->final_spacing_y = 0.0;
  p->final_spec_min_x = false;
  p->final_spec_min_y = false;
  p->final_spec_max_x = false;
  p->final_spec_max_y = false;
  p->final_spec_spacing_x = false;
  p->final_spec_spacing_y = false;
  p->final_transpose_axes = false;

  p->points_length = 1024;
  p->no_of_points = 0;
  p->reposition_trans_x = 0.0;
  p->reposition_trans_y = 0.0;
  p->reposition_scale = 1.0;
  p->frame_on_top = false;
}

static int
_scm_is_graph (SCM x)
{
  if (SCM_SMOB_PREDICATE (graph_tag, x))
    {
      if (SCM_SMOB_DATA (x) != 0)
	return 1;
      else
	return 0;
    }
  else
    return 0;

}

static graph_t *
_scm_to_graph (SCM x)
{
  return (graph_t *) SCM_SMOB_DATA (x);
}

SCM
equalp_graph (SCM x1, SCM x2)
{
  graph_t *graph1, *graph2;

  graph1 = (graph_t *) SCM_SMOB_DATA (x1);
  graph2 = (graph_t *) SCM_SMOB_DATA (x2);

  if ((graph1 == NULL) || (graph2 == NULL))
    return SCM_BOOL_F;
  else if (graph1 != graph2)
    return SCM_BOOL_F;
  else
    return SCM_BOOL_T;
}

SCM
mark_graph (SCM x)
{
  // No SCMs in the graph type: nothing to do here.
  return (SCM_BOOL_F);
}

size_t
free_graph (SCM x)
{
  graph_t *graph;

  graph = (graph_t *) SCM_SMOB_DATA (x);
  if (graph != NULL)
    {
      free (graph);
      SCM_SET_SMOB_DATA (x, 0);
    }

  return 0;
}

int
print_graph (SCM x, SCM port, scm_print_state * pstate)
{
  graph_t *graph = (graph_t *) SCM_SMOB_DATA (x);
  char *str;

  scm_puts ("#<graph ", port);

  if (graph == 0)
    scm_puts ( "(freed)", port);
  else
    {
      if (asprintf (&str, "%p", (void *) graph) < 0)
	scm_puts ("???", port);
      else
	scm_puts (str, port);
    }
  scm_puts (">", port);

  // non-zero means success
  return 1;
}

SCM
gupl_is_graph_p (SCM x)
{
  return scm_from_bool (_scm_is_graph (x));
}

SCM
gupl_new_graph (void)
{
  graph_t *c_graph;
  SCM s_graph;

  c_graph = (graph_t *) scm_malloc (sizeof (graph_t));
  initialize_struct_graph (c_graph);
  SCM_NEWSMOB (s_graph, graph_tag, c_graph);
  return (s_graph);
}

SCM
gupl_save_screen_x (SCM s_graph)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "save-screen!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->save_screen = true;
  return SCM_UNSPECIFIED;
}

SCM
gupl_toggle_transpose_axes_x (SCM s_graph)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "toggle-transpose-axes!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->transpose_axes = (c_graph->transpose_axes == true ? false : true);
  return SCM_UNSPECIFIED;
}

SCM
gupl_toggle_auto_bump_x (SCM s_graph)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "toggle-auto-bump!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->transpose_axes = (c_graph->transpose_axes == true ? false : true);
  return SCM_UNSPECIFIED;
}

SCM
gupl_toggle_use_color_x (SCM s_graph)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "toggle-use-color!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->new_use_color = true;
  c_graph->use_color = (c_graph->use_color == true ? false : true);
  return SCM_UNSPECIFIED;
}

SCM
gupl_toggle_frame_on_top_x (SCM s_graph)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "toggle-frame-on-top!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->frame_on_top = (c_graph->frame_on_top == true ? false : true);
  return SCM_UNSPECIFIED;
}

SCM
gupl_portable_output_x (SCM s_graph)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "portable-output!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->meta_portable = "yes";
  return SCM_UNSPECIFIED;
}

SCM
gupl_emulate_color_x (SCM s_graph, SCM s_str)
{
  graph_t *c_graph;
  char *c_str;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "emulate-color!");
  SCM_ASSERT (scm_is_string (s_str), s_str, SCM_ARG2, "emulate-color!");
  c_graph = _scm_to_graph (s_graph);
  c_str = scm_to_locale_string (s_str);
  c_graph->emulate_color = c_str;
  return SCM_UNSPECIFIED;
}

SCM
gupl_graph_version ()
{
  SCM p = scm_current_output_port ();

  scm_puts (progname, p);
  scm_puts (" (", p);
  scm_puts (PACKAGE_NAME, p);
  scm_puts (") ", p);
  scm_puts (PACKAGE_VERSION, p);
  scm_newline (p);

  scm_puts (copyright, p);
  scm_newline (p);

  scm_puts (written, p);
  scm_newline (p);
  return SCM_UNSPECIFIED;
}

SCM
gupl_toggle_rotate_y_label_x (SCM s_graph)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "toggle-rotate-y-label!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->no_rotate_y_label = (c_graph->no_rotate_y_label == true ? false : true);
  return SCM_UNSPECIFIED;
}

SCM
gupl_font_size_x (SCM s_graph, SCM s_font)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "font-size!");
  SCM_ASSERT (scm_is_real (s_font), s_font, SCM_ARG2, "font-size!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->local_font_size = scm_to_double (s_font);
  if (c_graph->local_font_size >= 1 || c_graph->local_font_size < 0.0)
    scm_out_of_range ("font-size", s_font);
  c_graph->font_size = c_graph->local_font_size;
  return SCM_UNSPECIFIED;
}

SCM
gupl_grid_style_x (SCM s_graph, SCM s_grid_style)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "grid-style!");
  SCM_ASSERT (scm_is_integer (s_grid_style), s_grid_style, SCM_ARG2, "grid-style!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->local_grid_style = scm_to_int (s_grid_style);
  if (c_graph->local_grid_style > 4 || c_graph->local_grid_style < 0)
    scm_out_of_range ("grid-style!", s_grid_style);
  switch (c_graph->local_grid_style)
    {
    case 0:
      c_graph->grid_spec = NO_AXES;
      break;
    case 1:
      c_graph->grid_spec = AXES;
      break;
    case 2:
      c_graph->grid_spec = AXES_AND_BOX;
      break;
    case 3:
      c_graph->grid_spec = AXES_AND_BOX_AND_GRID;
      break;
    case 4:
      c_graph->grid_spec = AXES_AT_ORIGIN;
      break;
    default:
      abort ();
    }
  return SCM_UNSPECIFIED;
}

SCM
gupl_height_of_plot_x (SCM s_graph, SCM s_height)
{
  graph_t *c_graph;
  double c_height;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "height-of-plot!");
  SCM_ASSERT (scm_is_real (s_height), s_height, SCM_ARG2, "height-of-plot!");
  c_graph = _scm_to_graph (s_graph);
  c_height = scm_to_double (s_height);
  c_graph->plot_height = c_height;
  return SCM_UNSPECIFIED;
}

SCM
gupl_clip_mode_x (SCM s_graph, SCM s_mode)
{
  graph_t *c_graph;
  int c_mode;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "clip-mode!");
  SCM_ASSERT (scm_is_integer (s_mode), s_mode, SCM_ARG2, "clip-mode!");
  c_graph = _scm_to_graph (s_graph);
  c_mode = scm_to_int (s_mode);
  c_graph->local_clip_mode = c_mode;
  if (c_mode < 0 || c_mode > 2)
    scm_out_of_range ("clip-mode!", s_mode);
  c_graph->clip_mode = c_mode;
  return SCM_UNSPECIFIED;
}

SCM
gupl_toggle_log_axis_x (SCM s_graph, SCM s_arg)
{
  graph_t *c_graph;
  size_t len;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "toggle-log-axis!");
  SCM_ASSERT (scm_is_string (s_arg), s_arg, SCM_ARG2, "toggle-log-axis!");
  c_graph = _scm_to_graph (s_graph);
  for (len = 0; len < scm_c_string_length (s_arg); len ++)
    {
      unsigned int c = SCM_CHAR (scm_c_string_ref (s_arg, len));
      if (c == 'x' || c == 'X')
	c_graph->log_axis ^= X_AXIS;
      else if (c == 'y' || c == 'Y')
	c_graph->log_axis ^= Y_AXIS;
    }
  return SCM_UNSPECIFIED;
}

SCM
gupl_toggle_no_ticks_x (SCM s_graph, SCM s_arg)
{
  graph_t *c_graph;
  size_t len;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "toggle-no-ticks!");
  SCM_ASSERT (scm_is_string (s_arg), s_arg, SCM_ARG2, "toggle-no-ticks!");
  c_graph = _scm_to_graph (s_graph);
  for (len = 0; len < scm_c_string_length (s_arg); len ++)
    {
      unsigned int c = SCM_CHAR (scm_c_string_ref (s_arg, len));
      if (c == 'x' || c == 'X')
	c_graph->omit_ticks ^= X_AXIS;
      else if (c == 'y' || c == 'Y')
	c_graph->omit_ticks ^= Y_AXIS;
    }
  return SCM_UNSPECIFIED;
}

SCM
gupl_line_mode_x (SCM s_graph, SCM s_index)
{
  graph_t *c_graph;
  int c_index;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "line-mode!");
  SCM_ASSERT (scm_is_integer (s_index), s_index, SCM_ARG2, "line-mode!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->new_linemode = true;
  c_index = scm_to_int (s_index);
  c_graph->linemode_index = c_index;
  return SCM_UNSPECIFIED;
}

SCM
gupl_fill_fraction_x (SCM s_graph, SCM s_fraction)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "fill-fraction!");
  SCM_ASSERT (scm_is_real (s_fraction), s_fraction, SCM_ARG2, "fill-fraction!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->local_fill_fraction = scm_to_double (s_fraction);
  if (c_graph->local_fill_fraction > 1.0)
    scm_out_of_range ("fill-fraction!", s_fraction);
  c_graph->fill_fraction = c_graph->local_fill_fraction;
  c_graph->new_fill_fraction = true;
  return SCM_UNSPECIFIED;
}

SCM
gupl_right_shift_x (SCM s_graph, SCM s_shift)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "right-shift!");
  SCM_ASSERT (scm_is_real (s_shift), s_shift, SCM_ARG2, "right-shift!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->margin_left = scm_to_double (s_shift);
  return SCM_UNSPECIFIED;
}

SCM
gupl_upward_shift_x (SCM s_graph, SCM s_shift)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "upward-shift!");
  SCM_ASSERT (scm_is_real (s_shift), s_shift, SCM_ARG2, "upward-shift!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->margin_below = scm_to_double (s_shift);
  return SCM_UNSPECIFIED;
}

SCM
gupl_width_of_plot_x (SCM s_graph, SCM s_width)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "width-of-plot!");
  SCM_ASSERT (scm_is_real (s_width), s_width, SCM_ARG2, "width-of-plot!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->plot_width = scm_to_double (s_width);
  return SCM_UNSPECIFIED;
}


SCM
gupl_output_format_x (SCM s_graph, SCM s_format)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "output-format!");
  SCM_ASSERT (scm_is_string (s_format), s_format, SCM_ARG2, "output-format!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->output_format = scm_to_locale_string (s_format);
  return SCM_UNSPECIFIED;
}

SCM
gupl_font_name_x (SCM s_graph, SCM s_name)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "font-name!");
  SCM_ASSERT (scm_is_string (s_name), s_name, SCM_ARG2, "font-name!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->font_name = scm_to_locale_string (s_name);
  return SCM_UNSPECIFIED;
}

SCM
gupl_rotation_angle_x (SCM s_graph, SCM s_angle)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "rotation-angle!");
  SCM_ASSERT (scm_is_string (s_angle), s_angle, SCM_ARG2, "rotation-angle!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->rotation_angle = scm_to_locale_string (s_angle);
  return SCM_UNSPECIFIED;
}

SCM
gupl_title_font_name_x (SCM s_graph, SCM s_name)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "title-font-name!");
  SCM_ASSERT (scm_is_string (s_name), s_name, SCM_ARG2, "title-font-name!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->title_font_name = scm_to_locale_string (s_name);
  return SCM_UNSPECIFIED;
}

SCM
gupl_symbol_font_name_x (SCM s_graph, SCM s_name)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "symbol-font-name!");
  SCM_ASSERT (scm_is_string (s_name), s_name, SCM_ARG2, "symbol-font-name!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->symbol_font_name = scm_to_locale_string (s_name);
  return SCM_UNSPECIFIED;
}


SCM
gupl_toggle_round_to_next_tick_x (SCM s_graph, SCM s_arg)
{
  graph_t *c_graph;
  size_t len;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "toggle-round-to-next-tick!");
  SCM_ASSERT (scm_is_string (s_arg), s_arg, SCM_ARG2, "toggle-round-to-next-tick!");
  c_graph = _scm_to_graph (s_graph);
  for (len = 0; len < scm_c_string_length (s_arg); len ++)
    {
      unsigned int c = SCM_CHAR (scm_c_string_ref (s_arg, len));
      if (c == 'x' || c == 'X')
	c_graph->round_to_next_tick ^= X_AXIS;
      else if (c == 'y' || c == 'Y')
	c_graph->round_to_next_tick ^= Y_AXIS;
    }
  return SCM_UNSPECIFIED;
}

SCM
gupl_top_label_x (SCM s_graph, SCM s_label)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "top-label!");
  SCM_ASSERT (scm_is_string (s_label), s_label, SCM_ARG2, "top-label!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->top_label = scm_to_locale_string (s_label);
  return SCM_UNSPECIFIED;
}

SCM
gupl_tick_size_x (SCM s_graph, SCM s_val)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "tick-size!");
  SCM_ASSERT (scm_is_real (s_val), s_val, SCM_ARG2, "tick-size!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->tick_size = scm_to_double (s_val);
  return SCM_UNSPECIFIED;
}

SCM
gupl_line_width_x (SCM s_graph, SCM s_val)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "line-width!");
  SCM_ASSERT (scm_is_real (s_val), s_val, SCM_ARG2, "line-width!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->local_plot_line_width = scm_to_double (s_val);
  if (c_graph->local_plot_line_width < 0.0)
    scm_out_of_range ("line-width!", s_val);
  c_graph->plot_line_width = c_graph->local_plot_line_width;
  c_graph->new_plot_line_width = true;

  return SCM_UNSPECIFIED;
}

SCM
gupl_x_label_x (SCM s_graph, SCM s_label)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "x-label!");
  SCM_ASSERT (scm_is_string (s_label), s_label, SCM_ARG2, "x-label!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->x_label = scm_to_locale_string (s_label);
  return SCM_UNSPECIFIED;
}

SCM
gupl_y_label_x (SCM s_graph, SCM s_label)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "y-label!");
  SCM_ASSERT (scm_is_string (s_label), s_label, SCM_ARG2, "y-label!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->y_label = scm_to_locale_string (s_label);
  return SCM_UNSPECIFIED;
}

SCM
gupl_toggle_axis_end (SCM s_graph, SCM s_arg)
{
  graph_t *c_graph;
  size_t len;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "toggle-axis-end!");
  SCM_ASSERT (scm_is_string (s_arg), s_arg, SCM_ARG2, "toggle-axis-end!");
  c_graph = _scm_to_graph (s_graph);
  for (len = 0; len < scm_c_string_length (s_arg); len ++)
    {
      unsigned int c = SCM_CHAR (scm_c_string_ref (s_arg, len));
      if (c == 'x' || c == 'X')
	c_graph->switch_axis_end ^= X_AXIS;
      else if (c == 'y' || c == 'Y')
	c_graph->switch_axis_end ^= Y_AXIS;
    }
  return SCM_UNSPECIFIED;
}

SCM
gupl_blankout_x (SCM s_graph, SCM s_val)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "blankout!");
  SCM_ASSERT (scm_is_real (s_val), s_val, SCM_ARG2, "blankout!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->blankout_fraction = scm_to_double (s_val);
  return SCM_UNSPECIFIED;
}

SCM
gupl_bitmap_size_x (SCM s_graph, SCM s_val)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "bitmap-size!");
  SCM_ASSERT (scm_is_string (s_val), s_val, SCM_ARG2, "bitmap-size!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->bitmap_size = scm_to_locale_string (s_val);
  return SCM_UNSPECIFIED;
}

SCM
gupl_title_font_size_x (SCM s_graph, SCM s_font)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "title-font-size!");
  SCM_ASSERT (scm_is_real (s_font), s_font, SCM_ARG2, "title-font-size!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->title_font_size = scm_to_double (s_font);
  if (c_graph->local_title_font_size >= 1.0 || c_graph->local_font_size < 0.0)
    scm_out_of_range ("title-font-size!", s_font);
  c_graph->title_font_size = c_graph->local_title_font_size;
  return SCM_UNSPECIFIED;
}

SCM
gupl_frame_line_width_x (SCM s_graph, SCM s_val)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "frame-line-width!");
  SCM_ASSERT (scm_is_real (s_val), s_val, SCM_ARG2, "frame-line-width!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->local_frame_line_width = scm_to_double (s_val);
  if (c_graph->local_frame_line_width < 0.0)
    scm_out_of_range ("frame-line-width!", s_val);
  c_graph->frame_line_width = c_graph->local_frame_line_width;
  return SCM_UNSPECIFIED;
}

SCM
gupl_max_line_length_x (SCM s_graph, SCM s_val)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "max-line-length!");
  SCM_ASSERT (scm_is_string (s_val), s_val, SCM_ARG2, "max-line-length!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->max_line_length = scm_to_locale_string (s_val);
  return SCM_UNSPECIFIED;
}

SCM
gupl_page_size_x (SCM s_graph, SCM s_size)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "page-size!");
  SCM_ASSERT (scm_is_string (s_size), s_size, SCM_ARG2, "page-size!");
  c_graph = _scm_to_graph (s_graph);

  c_graph->page_size = scm_to_locale_string (s_size);
  return SCM_UNSPECIFIED;
}

SCM
gupl_pen_colors (SCM s_colors)
{
  char *c_colors;
  SCM_ASSERT (scm_is_string (s_colors), s_colors, SCM_ARG1, "pen-colors");
  c_colors = scm_to_locale_string (s_colors);
  return scm_from_bool (parse_pen_string (c_colors));
}

SCM
gupl_bg_color_x (SCM s_graph, SCM s_color)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "bg-color!");
  SCM_ASSERT (scm_is_string (s_color), s_color, SCM_ARG2, "bg-color!");
  c_graph = _scm_to_graph (s_graph);

  c_graph->bg_color = scm_to_locale_string (s_color);
  return SCM_UNSPECIFIED;
}

SCM
gupl_frame_color_x (SCM s_graph, SCM s_color)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "frame-color!");
  SCM_ASSERT (scm_is_string (s_color), s_color, SCM_ARG2, "frame-color!");
  c_graph = _scm_to_graph (s_graph);

  c_graph->frame_color = scm_to_locale_string (s_color);
  return SCM_UNSPECIFIED;
}

SCM
gupl_auto_abscissa_x (SCM s_graph, SCM s_delta_x, SCM s_x_start)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "auto-abscissa!");
  SCM_ASSERT (SCM_UNBNDP (s_delta_x) || scm_is_real (s_delta_x), s_delta_x,
	      SCM_ARG2, "auto-abscissa!");
  SCM_ASSERT (SCM_UNBNDP (s_x_start) || scm_is_real (s_x_start), s_x_start,
	      SCM_ARG3, "auto-abscissa!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->auto_abscissa = true;

  if (SCM_UNBNDP (s_delta_x))
    return SCM_UNSPECIFIED;

  c_graph->local_delta_x = scm_to_double (s_delta_x);
  if (c_graph->local_delta_x == 0.0)
    {
      c_graph->auto_abscissa = false;
      return SCM_UNSPECIFIED;
    }
  c_graph->delta_x = c_graph->local_delta_x;

  if (SCM_UNBNDP (s_x_start))
    return SCM_UNSPECIFIED;

  c_graph->local_x_start = scm_to_double (s_x_start);
  c_graph->x_start = c_graph->local_x_start;
  return SCM_UNSPECIFIED;
}

SCM
gupl_x_limits_x (SCM s_graph, SCM s_min, SCM s_max, SCM s_spacing)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "auto-abscissa!");

  c_graph = _scm_to_graph (s_graph);
  c_graph->matched = 0;

  if (SCM_UNBNDP (s_min) || scm_is_false (s_min))
    {
      c_graph->spec_min_x = false;
      c_graph->spec_max_x = false;
      c_graph->spec_spacing_x = false;
      return SCM_UNSPECIFIED;
    }
  c_graph->local_min_x = scm_to_double (s_min);
  c_graph->matched = 1;
  c_graph->spec_min_x = true;
  c_graph->min_x = c_graph->local_min_x;

  if (SCM_UNBNDP (s_max) || scm_is_false (s_max))
    {
      c_graph->spec_max_x = false;
      c_graph->spec_spacing_x = false;
      return SCM_UNSPECIFIED;
    }
  c_graph->local_max_x = scm_to_double (s_max);
  c_graph->matched = 2;
  c_graph->spec_max_x = true;
  c_graph->max_x = c_graph->local_max_x;

  if (SCM_UNBNDP (s_spacing) || scm_is_false (s_spacing))
    {
      c_graph->spec_spacing_x = false;
      return SCM_UNSPECIFIED;
    }
  c_graph->local_spacing_x = scm_to_double (s_spacing);
  c_graph->matched = 3;
  c_graph->spec_spacing_x = true;
  c_graph->spacing_x = c_graph->local_spacing_x;
  return SCM_UNSPECIFIED;
}

SCM
gupl_y_limits_x (SCM s_graph, SCM s_min, SCM s_max, SCM s_spacing)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "auto-abscissa!");

  c_graph = _scm_to_graph (s_graph);
  c_graph->matched = 0;

  if (SCM_UNBNDP (s_min) || scm_is_false (s_min))
    {
      c_graph->spec_min_y = false;
      c_graph->spec_max_y = false;
      c_graph->spec_spacing_y = false;
      return SCM_UNSPECIFIED;
    }
  c_graph->local_min_y = scm_to_double (s_min);
  c_graph->matched = 1;
  c_graph->spec_min_y = true;
  c_graph->min_y = c_graph->local_min_y;

  if (SCM_UNBNDP (s_max) || scm_is_false (s_max))
    {
      c_graph->spec_max_y = false;
      c_graph->spec_spacing_y = false;
      return SCM_UNSPECIFIED;
    }
  c_graph->local_max_y = scm_to_double (s_max);
  c_graph->matched = 2;
  c_graph->spec_max_y = true;
  c_graph->max_y = c_graph->local_max_y;

  if (SCM_UNBNDP (s_spacing) || scm_is_false (s_spacing))
    {
      c_graph->spec_spacing_y = false;
      return SCM_UNSPECIFIED;
    }
  c_graph->local_spacing_y = scm_to_double (s_spacing);
  c_graph->matched = 3;
  c_graph->spec_spacing_y = true;
  c_graph->spacing_y = c_graph->local_spacing_y;
  return SCM_UNSPECIFIED;
}

SCM
gupl_symbol_x (SCM s_graph, SCM s_index, SCM s_size)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "symbol!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->new_symbol = true;
  c_graph->symbol_index = 1;

  if (SCM_UNBNDP (s_index))
    return SCM_UNSPECIFIED;

  c_graph->local_symbol_index = scm_to_int (s_graph);
  if (c_graph->local_symbol_index < 0
      || c_graph->local_symbol_index > 255)
    scm_out_of_range ("symbol!", s_index);

  if (SCM_UNBNDP (s_size))
    return SCM_UNSPECIFIED;

  c_graph->local_symbol_size = scm_to_double (s_size);
  if (c_graph->local_symbol_size <= 0.0)
    scm_out_of_range ("symbol!", s_size);
  c_graph->symbol_size = c_graph->local_symbol_size;
  c_graph->new_symbol_size = true;
  return SCM_UNSPECIFIED;
}

SCM
gupl_reposition_x (SCM s_graph, SCM s_trans_x, SCM s_trans_y, SCM s_scale)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "reposition!");
  SCM_ASSERT (scm_is_real (s_trans_x), s_trans_x, SCM_ARG2, "reposition!");
  SCM_ASSERT (scm_is_real (s_trans_y), s_trans_y, SCM_ARG3, "reposition!");
  SCM_ASSERT (scm_is_real (s_scale), s_scale, SCM_ARG4, "reposition!");
  c_graph = _scm_to_graph (s_graph);
  c_graph->old_reposition_trans_x = c_graph->reposition_trans_x;
  c_graph->old_reposition_trans_y = c_graph->reposition_trans_y;
  c_graph->old_reposition_scale = c_graph->reposition_scale;
  c_graph->reposition_trans_x = scm_to_double (s_trans_x);
  c_graph->reposition_trans_y = scm_to_double (s_trans_y);
  c_graph->reposition_scale = scm_to_double (s_scale);

  if (!c_graph->first_file_of_graph)
    {
      if (!c_graph->filter)
	{
	  array_bounds (c_graph->p, c_graph->no_of_points,
			c_graph->final_transpose_axes,
			c_graph->clip_mode,
			&(c_graph->final_min_x),
			&(c_graph->final_min_y),
			&(c_graph->final_max_x),
			&(c_graph->final_max_y),
			c_graph->final_spec_min_x,
			c_graph->final_spec_min_y,
			c_graph->final_spec_max_x,
			c_graph->final_spec_max_y);
	  if (c_graph->first_graph_of_multigraph)
	    {
	      c_graph->multigrapher = new_multigrapher (c_graph->output_format,
							c_graph->bg_color,
							c_graph->bitmap_size,
							c_graph->emulate_color,
							c_graph->max_line_length,
							c_graph->meta_portable,
							c_graph->page_size,
							c_graph->rotation_angle,
							c_graph->save_screen);
	      if (c_graph->multigrapher == NULL)
		scm_misc_error ("reposition!",
				"the graphing device could not be opened",
				SCM_BOOL_F);
	    }
	  begin_graph (c_graph->multigrapher,
		       c_graph->old_reposition_scale,
		       c_graph->old_reposition_trans_x,
		       c_graph->old_reposition_trans_y);
	  if ((c_graph->title_font_name == NULL)
	      && (c_graph->font_name != NULL))
	    c_graph->title_font_name = c_graph->font_name;

	  set_graph_parameters (c_graph->multigrapher,
				c_graph->frame_line_width,
				c_graph->frame_color,
				c_graph->top_label,
				c_graph->title_font_name,
				c_graph->title_font_size,
				c_graph->tick_size,
				c_graph->grid_spec,
				c_graph->final_min_x,
				c_graph->final_max_x,
				c_graph->final_spacing_x,
				c_graph->final_min_y,
				c_graph->final_max_y,
				c_graph->final_spacing_y,
				c_graph->final_spec_spacing_x,
				c_graph->final_spec_spacing_y,
				c_graph->plot_width,
				c_graph->plot_height,
				c_graph->margin_below,
				c_graph->margin_left,
				c_graph->font_name,
				c_graph->font_size,
				c_graph->x_label,
				c_graph->font_name,
				c_graph->font_size,
				c_graph->y_label,
				c_graph->no_rotate_y_label,
				c_graph->final_log_axis,
				c_graph->final_round_to_next_tick,
				c_graph->switch_axis_end,
				c_graph->omit_ticks,
				c_graph->clip_mode,
				c_graph->blankout_fraction,
				c_graph->final_transpose_axes);

	  draw_frame_of_graph (c_graph->multigrapher,
			       (c_graph->first_graph_of_multigraph ? false : true));
	  plot_point_array (c_graph->multigrapher, c_graph->p,
			    c_graph->no_of_points);
	  free (c_graph->p);
	  c_graph->no_of_points = 0;
	}

      if (c_graph->frame_on_top)
	{
	  end_polyline_and_flush (c_graph->multigrapher);
	  draw_frame_of_graph (c_graph->multigrapher, false);
	}
      end_graph (c_graph->multigrapher);
      c_graph->first_graph_of_multigraph = false;
      c_graph->first_file_of_graph = true;
    }
  return SCM_UNSPECIFIED;
}

void
pre_finalize_axes_parameters (graph_t *c_graph)
{
  /* Set up the axes parameters, just in case the user has provided
     enough information about the plot so that the axes don't have to
     be computed automatically */

  if (c_graph->log_axis & X_AXIS)
    {
      if (c_graph->spec_min_x)
	{
	  if (c_graph->min_x > 0.0)
	    c_graph->min_x = log10(c_graph->min_x);
	  else
	    {
	      fprintf(stderr,
		      "%s: error: the limit %g on a logarithmic axis is nonpositive\n",
		      progname, c_graph->min_x);
	      // return EXIT_FAILURE;
	    }
	}
      if (c_graph->spec_max_x)
	{
	  if (c_graph->max_x > 0.0)
	    c_graph->max_x = log10(c_graph->max_x);
	  else
	    {
	      fprintf(stderr,
		      "%s: error: the limit %g on a logarithmic axis is nonpositive\n",
		      progname, c_graph->max_x);
	      //return EXIT_FAILURE;
	    }
	}
    }

  if (c_graph->log_axis & Y_AXIS)
    {
      if (c_graph->spec_min_y)
	{
	  if (c_graph->min_y > 0.0)
	    c_graph->min_y = log10(c_graph->min_y);
	  else
	    {
	      fprintf(stderr,
		      "%s: error: the limit %g on a logarithmic axis is nonpositive\n",
		      progname, c_graph->min_y);
	      // return EXIT_FAILURE;
	    }
	}
      if (c_graph->spec_max_y)
	{
	  if (c_graph->max_y > 0.0)
	    c_graph->max_y = log10(c_graph->max_y);
	  else
	    {
	      fprintf(stderr,
		      "%s: error: the limit %g on a logarithmic axis is nonpositive\n",
		      progname, c_graph->max_y);
	      //return EXIT_FAILURE;
	    }
	}
    }

  /* We now finalize the following parameters (arguments to
     set_graph_parameters()), even though we won't call
     set_graph_parameters() for a while yet, if it turns out we
     need to act as a real-time filter. */

  /* portmanteaux */
  c_graph->final_log_axis = c_graph->log_axis;
  c_graph->final_round_to_next_tick = c_graph->round_to_next_tick;

  /* bool */
  c_graph->final_transpose_axes = c_graph->transpose_axes;

  /* x-axis specific */
  c_graph->final_min_x = c_graph->min_x;
  c_graph->final_max_x = c_graph->max_x;
  c_graph->final_spacing_x = c_graph->spacing_x;
  c_graph->final_spec_min_x = c_graph->spec_min_x;
  c_graph->final_spec_max_x = c_graph->spec_max_x;
  c_graph->final_spec_spacing_x = c_graph->spec_spacing_x;

  /* y-axis specific */
  c_graph->final_min_y = c_graph->min_y;
  c_graph->final_max_y = c_graph->max_y;
  c_graph->final_spec_min_y = c_graph->spec_min_y;
  c_graph->final_spec_max_y = c_graph->spec_max_y;
  c_graph->final_spacing_y = c_graph->spacing_y;
  c_graph->final_spec_spacing_y = c_graph->spec_spacing_y;

  /* If user didn't specify either the lower limit or the upper
     limit for an axis, by default we'll round the axis limits
     to the nearest tick, after computing them.  (If either
     limit was specified by the user, to request rounding the
     user must specify the -R option as well.) */
  if (!c_graph->final_spec_min_x && !c_graph->final_spec_max_x)
    c_graph->final_round_to_next_tick |= X_AXIS;
  if (!c_graph->final_spec_min_y && !c_graph->final_spec_max_y)
    c_graph->final_round_to_next_tick |= Y_AXIS;

  /* The case when x_min, x_max, y_min, y_max are all specified
     by the luser is special: we set the `filter' flag for the
     duration of this plot.  This indicates that the axes information
     does not need to be computed from the data.  */
  c_graph->filter = ((c_graph->final_spec_min_x && c_graph->final_spec_max_x
	     && c_graph->final_spec_min_y && c_graph->final_spec_max_y)
	    ? true : false);
}


VecReader *
new_vec_reader (SCM s_xvec, SCM s_yvec, SCM s_yerrvec, data_type format_type,
		bool auto_abscissa, double delta_x, double abscissa,
		bool transpose_axes, int log_axis, bool auto_bump, int symbol,
		double symbol_size, const char *symbol_font_name, int linemode,
		double line_width, double fill_fraction, bool use_color)

{
  VecReader *reader;

  reader = (VecReader *)xmalloc (sizeof (VecReader));

  reader->need_break = true;	/* next point will have pen up */
  reader->xvec = s_xvec;
  reader->yvec = s_yvec;
  reader->yerrvec = s_yerrvec;
  reader->i_vec = 0;
  reader->format_type = format_type;
  reader->auto_abscissa = auto_abscissa;
  reader->delta_x = delta_x;
  reader->initial_abscissa = abscissa;
  reader->abscissa = reader->initial_abscissa;
  reader->transpose_axes = transpose_axes;
  reader->log_axis = log_axis;
  reader->auto_bump = auto_bump;
  reader->symbol = symbol;
  reader->symbol_size = symbol_size;
  reader->symbol_font_name = symbol_font_name;
  reader->linemode = linemode;
  reader->line_width = line_width;
  reader->fill_fraction = fill_fraction;
  reader->use_color = use_color;

  return reader;
}


void
alter_vec_reader_parameters (VecReader *reader, SCM s_xvec, SCM s_yvec, SCM s_yerrvec,
			     data_type format_type, bool auto_abscissa, double delta_x,
			     double abscissa, int symbol, double symbol_size,
			     const char *symbol_font_name, int linemode,
			     double line_width, double fill_fraction, bool use_color,
			     bool new_symbol, bool new_symbol_size,
			     bool new_symbol_font_name, bool new_linemode,
			     bool new_line_width, bool new_fill_fraction,
			     bool new_use_color)
{
  reader->need_break = true;	/* force break in polyline */
  reader->xvec = s_xvec;
  reader->yvec = s_yvec;
  reader->yerrvec = s_yerrvec;
  reader->format_type = format_type;
  reader->auto_abscissa = auto_abscissa;
  reader->delta_x = delta_x;
  reader->initial_abscissa = abscissa;
  reader->abscissa = reader->initial_abscissa;
  /* test bits in mask to determine which polyline attributes need updating */
  if (new_symbol)
    reader->symbol = symbol;
  if (new_symbol_size)
    reader->symbol_size = symbol_size;
  if (new_symbol_font_name)
    reader->symbol_font_name = symbol_font_name;
  if (new_linemode)
    reader->linemode = linemode;
  if (new_line_width)
    reader->line_width = line_width;
  if (new_fill_fraction)
    reader->fill_fraction = fill_fraction;
  if (new_use_color)
    reader->use_color = use_color;

  return;
}

static void
reset_vec_reader (VecReader *reader)
{
  reader->need_break = true;	/* force break in polyline */

  /* bump linemode if appropriate */
  if (reader->auto_bump)
    reader->linemode += ((reader->linemode > 0) ? 1 : -1);

  /* reset abscissa if auto-abscissa is in effect */
  if (reader->auto_abscissa)
    reader->abscissa = reader->initial_abscissa;

  return;
}

void
read_vector (VecReader *reader, Point **p_addr, int *length, int *no_of_points)
{
  dataset_status_t status;

  do
    {
      status = read_vector_dataset (reader, p_addr, length, no_of_points);

      /* After each dataset, reset reader: force break in polyline, bump
	 linemode (if auto-bump is in effect), and reset abscissa (if
	 auto-abscissa is in effect).  But if dataset ended with an
	 explicit set linemode / symbol style directive, don't bump the
	 linemode. */
      if (status == ENDED_BY_MODE_CHANGE)
	{
	  bool saved_auto_bump;

	  saved_auto_bump = reader->auto_bump;
	  reader->auto_bump = false;
	  reset_vec_reader (reader);
	  reader->auto_bump = saved_auto_bump;
	}
      else
	reset_vec_reader (reader);
    }
  while (status != ENDED_BY_EOF);
}

static dataset_status_t
read_vector_dataset (VecReader *reader, Point **p_addr, int *length, int *no_of_points)
{
  Point *p = *p_addr;
  dataset_status_t status;

  for ( ; ; )
    {
      /*
       * Grow the buffer if needed
       */
      if (*no_of_points >= *length)
	{
	  int old_length, new_length;

	  old_length = *length;
	  new_length = NEW_POINT_ARRAY_LENGTH(old_length);
	  p = (Point *)xrealloc (p, new_length * sizeof (Point));
	  *length = new_length;
	}

      status = read_vector_point (reader, &(p[*no_of_points]));
      if (status != IN_PROGRESS)
	/* we didn't get a point, i.e. dataset ended */
	break;

      (*no_of_points)++;
    }

  *p_addr = p;			/* update beginning of array if needed */

  return status;
}

static dataset_status_t
read_vector_point (VecReader *reader, Point *point)
{
  dataset_status_t status;

  /* following fields are constant throughout each polyline */
  point->symbol = reader->symbol;
  point->symbol_size = reader->symbol_size;
  point->symbol_font_name = reader->symbol_font_name;
  point->linemode = reader->linemode;
  point->line_width = reader->line_width;
  point->fill_fraction = reader->fill_fraction;
  point->use_color = reader->use_color;
  point->have_x_errorbar = false; /* not supported yet */
  point->have_y_errorbar = (reader->format_type == T_ASCII_ERRORBAR ? true : false);

 head:

  if (scm_is_true (scm_list_p (reader->yvec)))
    status = read_point_from_list (reader, point);
  //else if (scm_is_true (scm_f64vector_p (reader->yvec)))
  //status = read_point_from_f64vector (reader, point);
  else
    scm_misc_error (NULL, "bad format", SCM_BOOL_F);

  if (status == IN_PROGRESS)
    /* got a point; if not, we just pass back the return code */
    {
      bool bad_point = false;

      /* If we have log axes, the values we work with ALL refer to the log10
	 values of the data.  A nonpositive value generates a warning, and a
	 break in the polyline. */
      if (reader->log_axis & X_AXIS)
	{
	  if (point->x > 0.0)
	    point->x = log10 (point->x);
	  else
	    bad_point = true;
	  if (point->have_x_errorbar)
	    {
	      if (point->xmin > 0.0)
		point->xmin = log10 (point->xmin);
	      else
		bad_point = true;
	      if (point->xmax > 0.0)
		point->xmax = log10 (point->xmax);
	      else
		bad_point = true;
	    }

	  if (bad_point)
	    {
	      fprintf (stderr, "%s: the inappropriate point (%g,%g) is dropped, as this is a log plot\n",
		       progname, point->x, point->y);
	      reader->need_break = true;
	      goto head;		/* on to next point */
	    }
	}
      if (reader->log_axis & Y_AXIS)
	{
	  if (point->y > 0.0)
	    point->y = log10 (point->y);
	  else
	    bad_point = true;

	  if (point->have_y_errorbar)
	    {
	      if (point->ymin > 0.0)
		point->ymin = log10 (point->ymin);
	      else
		bad_point = true;
	      if (point->ymax > 0.0)
		point->ymax = log10 (point->ymax);
	      else
		bad_point = true;
	    }

	  if (bad_point)
	    {
	      fprintf (stderr, "%s: the inappropriate point (%g,%g) is dropped, as this is a log plot\n",
		       progname, point->x, point->y);
	      reader->need_break = true;
	      goto head;		/* on to next point */
	    }
	}

      if (reader->transpose_axes)
	{
	  double tmp;
	  bool tmp_bool;

	  tmp = point->x;
	  point->x = point->y;
	  point->y = tmp;
	  tmp = point->xmin;
	  point->xmin = point->ymin;
	  point->ymin = tmp;
	  tmp = point->xmax;
	  point->xmax = point->ymax;
	  point->ymax = tmp;
	  tmp_bool = point->have_x_errorbar;
	  point->have_x_errorbar = point->have_y_errorbar;
	  point->have_y_errorbar = tmp_bool;
	}

      /* we have a point, but we may need to break the polyline before it */
      if (reader->need_break)
	point->pendown = false;
      else
	point->pendown = true;

      /* reset break-polyline flag */
      reader->need_break = false;
    }

  return status;
}

static dataset_status_t
read_point_from_list (VecReader *reader, Point *point)
{
  size_t i = reader->i_vec;

  reader->i_vec ++;

  /* read coordinate(s) */
  if (reader->auto_abscissa)
    {
      point->x = reader->abscissa;
      reader->abscissa += reader->delta_x;
    }
  else
    {
      size_t x_len = scm_to_size_t (scm_length (reader->xvec));
      SCM sx;

      if (i >= x_len)
	return ENDED_BY_EOF;
      sx = scm_list_ref (reader->xvec, scm_from_size_t (i));
      if (sx == SCM_BOOL_F)
	return ENDED_BY_DATASET_TERMINATOR;
      point->x = scm_to_double (sx);
    }

  {
      size_t y_len = scm_to_size_t (scm_length (reader->yvec));
      SCM sy;

      if (i >= y_len)
	return ENDED_BY_EOF;
      sy = scm_list_ref (reader->yvec, scm_from_size_t (i));
      if (sy == SCM_BOOL_F)
	return ENDED_BY_DATASET_TERMINATOR;
      point->y = scm_to_double (sy);
  }

  /* Check for the optional y error bar data */
  if (scm_is_true (scm_list_p (reader->yerrvec)))
    {
      size_t yerr_len = scm_to_size_t (scm_length (reader->yerrvec));
      SCM syerr;
      double error_size;

      if (i >= yerr_len)
	{
	  point->have_y_errorbar = false;
	}
      else
	{
	  syerr = scm_list_ref (reader->yerrvec, scm_from_size_t (i));
	  if (syerr == SCM_BOOL_F)
	    {
	      point->have_y_errorbar = false;
	    }
	  else
	    {
	      point->have_y_errorbar = true;
	      error_size = scm_to_double (syerr);
	      point->ymin = point->y - error_size;
	      point->ymax = point->y + error_size;
	    }
	}
    }
  else
    point->have_y_errorbar = false;

  return IN_PROGRESS;
}


void
copy_xy_vectors_to_points_array (SCM xvec, SCM yvec, Reader *reader,
				 Point **p_addr, int *length, int *no_of_points)
{
  size_t i_vec = 0;
}

SCM
gupl_xy_data_x (SCM s_graph, SCM s_xvec, SCM s_yvec)
{
  graph_t *c_graph;
  size_t i, len;

  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "xy-graph!");
  //SCM_ASSERT (scm_is_true (scm_f64vector_p (s_xvec)), s_xvec, SCM_ARG2,
  //"xy-graph!");
//SCM_ASSERT (scm_is_true (scm_f64vector_p (s_yvec)), s_yvec, SCM_ARG3,
  // "xy-graph!");

  c_graph = _scm_to_graph (s_graph);

  if (c_graph->first_file_of_graph)
    {
      pre_finalize_axes_parameters (c_graph);
    }

  /* By now, the filter flag is set.  If the filter flag is true, we don't
     need to compute the axis parameters, since they are fully specified.  But,
     we're not doing anything with filter, yet.  FIXME.  */

  if (c_graph->first_file_of_graph)
    {
      c_graph->p = scm_malloc (c_graph->points_length * sizeof (Point));

      c_graph->reader = new_vec_reader (s_xvec,
					s_yvec,
					SCM_BOOL_F, /* y error bars */
					c_graph->input_type,
					c_graph->auto_abscissa, c_graph->delta_x,
					c_graph->x_start, c_graph->final_transpose_axes,
					c_graph->final_log_axis, c_graph->auto_bump,
					c_graph->symbol_index, c_graph->symbol_size,
					c_graph->symbol_font_name,
					c_graph->linemode_index,
					c_graph->plot_line_width,
					c_graph->fill_fraction,
					c_graph->use_color);

      c_graph->new_symbol = false;
      c_graph->new_symbol_size = false;
      c_graph->new_symbol_font_name = false;
      c_graph->new_linemode = false;
      c_graph->new_plot_line_width = false;
      c_graph->new_fill_fraction = false;
      c_graph->new_use_color = false;
    }
  else
    {
      alter_vec_reader_parameters (c_graph->reader,
			       s_xvec,
			       s_yvec,
			       SCM_BOOL_F,
			       c_graph->input_type,
			       c_graph->auto_abscissa, c_graph->delta_x,
			       c_graph->x_start, c_graph->symbol_index,
			       c_graph->symbol_size, c_graph->symbol_font_name,
			       c_graph->linemode_index, c_graph->plot_line_width,
			       c_graph->fill_fraction, c_graph->use_color,
			       c_graph->new_symbol, c_graph->new_symbol_size,
			       c_graph->new_symbol_font_name, c_graph->new_linemode,
			       c_graph->new_plot_line_width, c_graph->new_fill_fraction,
			       c_graph->new_use_color);
      c_graph->new_symbol = false;
      c_graph->new_symbol_size = false;
      c_graph->new_symbol_font_name = false;
      c_graph->new_linemode = false;
      c_graph->new_plot_line_width = false;
      c_graph->new_fill_fraction = false;
      c_graph->new_use_color = false;
    }

  read_vector (c_graph->reader,
	       &(c_graph->p), &(c_graph->points_length),
	       &(c_graph->no_of_points));

  c_graph->first_file_of_graph = false;


  return SCM_UNSPECIFIED;
}

SCM
gupl_generate (SCM s_graph)
{
  graph_t *c_graph;
  SCM_ASSERT (_scm_is_graph (s_graph), s_graph, SCM_ARG1, "generate");
  c_graph = _scm_to_graph (s_graph);
  if (c_graph->first_file_of_graph == false)
    {
      /* fill in any of min_? and max_? that user didn't specify (the
	 prefix "final_" means these arguments were finalized at the
	 time the first file of the plot was processed) */
      array_bounds (c_graph->p,
		    c_graph->no_of_points,
		    c_graph->final_transpose_axes,
		    c_graph->clip_mode,
		    &(c_graph->final_min_x),
		    &(c_graph->final_min_y),
		    &(c_graph->final_max_x),
		    &(c_graph->final_max_y),
		    c_graph->final_spec_min_x,
		    c_graph->final_spec_min_y,
		    c_graph->final_spec_max_x,
		    c_graph->final_spec_max_y);

      if (c_graph->first_graph_of_multigraph)
	/* still haven't created multigrapher, do so now */
	{
	  if ((c_graph->multigrapher
	       = new_multigrapher (c_graph->output_format,
				   c_graph->bg_color,
				   c_graph->bitmap_size,
				   c_graph->emulate_color,
				   c_graph->max_line_length,
				   c_graph->meta_portable,
				   c_graph->page_size,
				   c_graph->rotation_angle,
				   c_graph->save_screen)) == NULL)
	    {
	      fprintf (stderr,
		       "%s: error: the graphing device could not be opened\n", progname);
	      // return EXIT_FAILURE;
	    }
	}

      /* begin graph: push new libplot drawing state onto stack of
	 states; also concatenate the current transformation matrix
	 with a matrix formed from the repositioning parameters (this
	 will take effect for the duration of the graph) */
      begin_graph (c_graph->multigrapher,
		   c_graph->reposition_scale,
		   c_graph->reposition_trans_x,
		   c_graph->reposition_trans_y);

      /* font selection, saves typing */
      if ((c_graph->title_font_name == NULL) && (c_graph->font_name != NULL))
	c_graph->title_font_name = c_graph->font_name;

      set_graph_parameters (c_graph->multigrapher,
			    c_graph->frame_line_width,
			    c_graph->frame_color,
			    c_graph->top_label,
			    c_graph->title_font_name,
			    c_graph->title_font_size, /*for title*/
			    c_graph->tick_size,
			    c_graph->grid_spec,
			    c_graph->final_min_x,
			    c_graph->final_max_x,
			    c_graph->final_spacing_x,
			    c_graph->final_min_y,
			    c_graph->final_max_y,
			    c_graph->final_spacing_y,
			    c_graph->final_spec_spacing_x,
			    c_graph->final_spec_spacing_y,
			    c_graph->plot_width,
			    c_graph->plot_height,
			    c_graph->margin_below,
			    c_graph->margin_left,
			    c_graph->font_name,
			    c_graph->font_size, /* for abscissa label */
			    c_graph->x_label,
			    c_graph->font_name,
			    c_graph->font_size, /* for ordinate label */
			    c_graph->y_label,
			    c_graph->no_rotate_y_label,
			    /* these args are portmanteaux */
			    c_graph->final_log_axis,
			    c_graph->final_round_to_next_tick,
			    c_graph->switch_axis_end,
			    c_graph->omit_ticks,
			    /* more args */
			    c_graph->clip_mode,
			    c_graph->blankout_fraction,
			    c_graph->final_transpose_axes);

      /* draw the graph frame (grid, ticks, etc.); draw a `canvas' (a
	 background opaque white rectangle) only if this isn't the
	 first graph */
      draw_frame_of_graph (c_graph->multigrapher,
			   c_graph->first_graph_of_multigraph ? false : true);

      /* plot the laboriously read-in array */
      plot_point_array (c_graph->multigrapher, c_graph->p,
			c_graph->no_of_points);

      /* free points array */
      free (c_graph->p);
      c_graph->no_of_points = 0;

      /* draw graph frame on top of graph, if user requested it */
      if (c_graph->frame_on_top)
	{
	  end_polyline_and_flush (c_graph->multigrapher);
	  draw_frame_of_graph (c_graph->multigrapher, false);
	}

      /* end graph: pop drawing state off the stack of drawing states */
      end_graph (c_graph->multigrapher);

    } /* end of nonempty-graph case */

  /* finish up by deleting our multigrapher (one must have been created,
     since we always read at least stdin) */
  if (delete_multigrapher (c_graph->multigrapher) < 0)
    {
      fprintf (stderr, "%s: error: the graphing device could not be closed\n",
	       progname);
      // return EXIT_FAILURE;
    }
  return SCM_UNSPECIFIED;
}

static bool
parse_pen_string (const char *pen_s)
{
  const char *charp;
  char name[MAX_COLOR_NAME_LEN];
  int i;

  charp = pen_s;
  while (*charp)
    {
      int pen_num;
      bool got_digit;
      const char *tmp;

      if (*charp == ':')	/* skip any ':' */
	{
	  charp++;
	  continue;		/* back to top of while loop */
	}
      pen_num = 0;
      got_digit = false;
      while (*charp >= '0' && *charp <= '9')
	{
	  pen_num = 10 * pen_num + (int)*charp - (int)'0';
	  got_digit = true;
	  charp++;
	}
      if (!got_digit || pen_num < 1 || pen_num > NO_OF_LINEMODES)
	return false;
      if (*charp != '=')
	return false;
      charp++;
      for (tmp = charp, i = 0; i < MAX_COLOR_NAME_LEN; tmp++, i++)
	{
	  if (*tmp == ':') /* end of color name string */
	    {
	      name[i] = '\0';
	      charp = tmp + 1;
	      break;
	    }
	  else if (*tmp == '\0') /* end of name string */
	    {
	      name[i] = '\0';
	      charp = tmp;
	      break;
	    }
	  else
	    name[i] = *tmp;
	}

      /* replace pen color name by user-specified color name */
      colorstyle[pen_num - 1] = xstrdup (name);
    }
  return true;
}

void libguile_plotutils_LTX_gupl_graph_init (void)
{
  gupl_graph_init ();
}

void
gupl_graph_init (void)
{

  graph_tag = scm_make_smob_type ("graph", sizeof (graph_t *));
  scm_set_smob_mark (graph_tag, mark_graph);
  scm_set_smob_free (graph_tag, free_graph);
  scm_set_smob_print (graph_tag, print_graph);
  scm_set_smob_equalp (graph_tag, equalp_graph);

  scm_c_define_gsubr ("graph?", 1, 0, 0, gupl_is_graph_p);
  scm_c_define_gsubr ("new-graph", 0, 0, 0, gupl_new_graph);
  scm_c_define_gsubr ("save-screen!", 1, 0, 0, gupl_save_screen_x);
  scm_c_define_gsubr ("toggle-transpose-axes!", 1, 0, 0,
		      gupl_toggle_transpose_axes_x);
  scm_c_define_gsubr ("toggle-auto-bump!", 1, 0, 0, gupl_toggle_auto_bump_x);
  scm_c_define_gsubr ("toggle-use-color!", 1, 0, 0, gupl_toggle_use_color_x);
  scm_c_define_gsubr ("toggle-frame-on-top!", 1, 0, 0, gupl_toggle_frame_on_top_x);
  scm_c_define_gsubr ("portable-output!", 1, 0, 0, gupl_portable_output_x);
  scm_c_define_gsubr ("emulate-color!", 2, 0, 0, gupl_emulate_color_x);
  scm_c_define_gsubr ("graph-version", 0, 0, 0, gupl_graph_version);
  scm_c_define_gsubr ("toggle-rotate-y-label!", 1, 0, 0,
		      gupl_toggle_rotate_y_label_x);
  scm_c_define_gsubr ("font-size!", 2, 0, 0, gupl_font_size_x);
  scm_c_define_gsubr ("grid-style!", 2, 0, 0, gupl_grid_style_x);
  scm_c_define_gsubr ("height-of-plot!", 2, 0, 0, gupl_height_of_plot_x);
  scm_c_define_gsubr ("clip-mode!", 2, 0, 0, gupl_clip_mode_x);
  scm_c_define_gsubr ("toggle-log-axis!", 2, 0, 0, gupl_toggle_log_axis_x);
  scm_c_define_gsubr ("toggle-no-ticks!", 2, 0, 0, gupl_toggle_no_ticks_x);
  scm_c_define_gsubr ("line-mode!", 2, 0, 0, gupl_line_mode_x);
  scm_c_define_gsubr ("fill-fraction!", 2, 0, 0, gupl_fill_fraction_x);
  scm_c_define_gsubr ("right-shift!", 2, 0, 0, gupl_right_shift_x);
  scm_c_define_gsubr ("upward-shift!", 2, 0, 0, gupl_upward_shift_x);
  scm_c_define_gsubr ("width-of-plot!", 2, 0, 0, gupl_width_of_plot_x);
  scm_c_define_gsubr ("output-format!", 2, 0, 0, gupl_output_format_x);
  scm_c_define_gsubr ("font-name!", 2, 0, 0, gupl_font_name_x);
  scm_c_define_gsubr ("rotation-angle!", 2, 0, 0, gupl_rotation_angle_x);
  scm_c_define_gsubr ("title-font-name!", 2, 0, 0, gupl_title_font_name_x);
  scm_c_define_gsubr ("symbol-font-name!", 2, 0, 0, gupl_symbol_font_name_x);
  scm_c_define_gsubr ("toggle-round-to-next-tick!", 2, 0, 0,
		      gupl_toggle_round_to_next_tick_x);
  scm_c_define_gsubr ("top-label!", 2, 0, 0, gupl_top_label_x);
  scm_c_define_gsubr ("tick-size!", 2, 0, 0, gupl_tick_size_x);
  scm_c_define_gsubr ("line-width!", 2, 0, 0, gupl_line_width_x);
  scm_c_define_gsubr ("x-label!", 2, 0, 0, gupl_x_label_x);
  scm_c_define_gsubr ("y-label!", 2, 0, 0, gupl_y_label_x);
  scm_c_define_gsubr ("toggle-axis-end!", 2, 0, 0, gupl_toggle_axis_end);
  scm_c_define_gsubr ("blankout!", 2, 0, 0, gupl_blankout_x);
  scm_c_define_gsubr ("bitmap-size!", 2, 0, 0, gupl_bitmap_size_x);
  scm_c_define_gsubr ("title-font-size!", 2, 0, 0, gupl_title_font_size_x);
  scm_c_define_gsubr ("frame-line-width!", 2, 0, 0, gupl_frame_line_width_x);
  scm_c_define_gsubr ("max-line-length!", 2, 0, 0, gupl_max_line_length_x);
  scm_c_define_gsubr ("page-size!", 2, 0, 0, gupl_page_size_x);
  scm_c_define_gsubr ("pen-colors", 1, 0, 0, gupl_pen_colors);
  scm_c_define_gsubr ("bg-color!", 2, 0, 0, gupl_bg_color_x);
  scm_c_define_gsubr ("frame-color!", 2, 0, 0, gupl_frame_color_x);
  scm_c_define_gsubr ("auto-abscissa!", 1, 2, 0, gupl_auto_abscissa_x);
  scm_c_define_gsubr ("x-limits!", 1, 3, 0, gupl_x_limits_x);
  scm_c_define_gsubr ("y-limits!", 1, 3, 0, gupl_y_limits_x);
  scm_c_define_gsubr ("symbol!", 1, 2, 0, gupl_symbol_x);
  scm_c_define_gsubr ("reposition!", 4, 0, 0, gupl_reposition_x);
  scm_c_define_gsubr ("xy-data!", 3, 0, 0, gupl_xy_data_x);
  scm_c_define_gsubr ("generate", 1, 0, 0, gupl_generate);






  //scm_c_define_gsubr ("graph", 20, 0, 0, gupl_graph);
}
