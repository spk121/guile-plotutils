#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE
#include <stdio.h>


#include <assert.h>
#include <libguile.h>
#include <plot.h>
#include <unistd.h>
#include "guile_plot.h"

#define PORT_ERR (-1)
#define PORT_OK (0)

static scm_t_bits plotter_tag;
static scm_t_bits plparams_tag;

static int _scm_is_plparams (SCM x);
static plPlotterParams *_scm_to_plparams (SCM x);
static SCM _scm_from_plparams (plPlotterParams *x);
static SCM equalp_plparams (SCM x1, SCM x2);
static SCM mark_plparams (SCM x);
static size_t free_plparams (SCM x);
static int print_plparams (SCM x, SCM port, scm_print_state * pstate);

static int _scm_is_plotter (SCM x);
static plPlotter *_scm_to_plotter (SCM x);
static SCM _scm_from_plotter (plPlotter *x);
static SCM equalp_plotter (SCM x1, SCM x2);
static SCM mark_plotter (SCM x);
static size_t free_plotter (SCM x);
static int print_plotter (SCM x, SCM port, scm_print_state * pstate);

static ssize_t port_read (void *cookie, char *buf, size_t siz);
static ssize_t port_write (void *cookie, const char *buf, size_t siz);
static int port_seek (void *cookie, off64_t * pos, int whence);
static int port_close (void *cookie);

static cookie_io_functions_t port_funcs;

static int _scm_is_exact_integer (SCM x);

int
_scm_is_plparams (SCM x)
{
  if (SCM_SMOB_PREDICATE (plparams_tag, x))
    {
      if (SCM_SMOB_DATA (x) != 0)
	return 1;
      else
	return 0;
    }
  else
    return 0;

}

plPlotterParams *
_scm_to_plparams (SCM x)
{
  assert (_scm_is_plparams (x));

  return (plPlotterParams *) SCM_SMOB_DATA (x);
}

SCM
_scm_from_plparams (plPlotterParams * x)
{
  SCM s_plparams;

  assert (x != NULL);

  SCM_NEWSMOB (s_plparams, plparams_tag, x);

  assert (x == (plPlotterParams *) SCM_SMOB_DATA (s_plparams));

  return (s_plparams);
}

SCM
equalp_plparams (SCM x1, SCM x2)
{
  plPlotterParams *plparams1, *plparams2;

  plparams1 = (plPlotterParams *) SCM_SMOB_DATA (x1);
  plparams2 = (plPlotterParams *) SCM_SMOB_DATA (x2);

  if ((plparams1 == NULL) || (plparams2 == NULL))
    return SCM_BOOL_F;
  else if (plparams1 != plparams2)
    return SCM_BOOL_F;
  else
    return SCM_BOOL_T;
}

SCM
mark_plparams (SCM x)
{
  // No SCMs in the plparams type: nothing to do here.
  return (SCM_BOOL_F);
}

size_t
free_plparams (SCM x)
{
  plPlotterParams *plparams;

  // assert (SCM_SMOB_PREDICATE (plparams_tag, x));

  plparams = (plPlotterParams *) SCM_SMOB_DATA (x);
  if (plparams != NULL)
    {
      pl_deleteplparams (plparams);
      SCM_SET_SMOB_DATA (x, 0);
    }

  return 0;
}

int
print_plparams (SCM x, SCM port, scm_print_state * pstate)
{
  plPlotterParams *plparams = (plPlotterParams *) SCM_SMOB_DATA (x);
  char *str;

  assert (SCM_SMOB_PREDICATE (plotter_tag, x));

  scm_puts ("#<plparams ", port);

  if (plparams == 0)
    scm_puts ( "(freed)", port);
  else
    {
      if (asprintf (&str, "%p", (void *) plparams) < 0)
	scm_puts ("???", port);
      else
	scm_puts (str, port);
    }
  scm_puts (">", port);

  // non-zero means success
  return 1;
}

SCM
gupl_is_plparams_p (SCM x)
{
  return scm_from_bool (_scm_is_plotter (x));
}

SCM
gupl_newplparams (void)
{
  plPlotterParams *c_plparams;
  c_plparams = pl_newplparams ();
  return _scm_from_plparams (c_plparams);
}

int
_scm_is_plotter (SCM x)
{
  if (SCM_SMOB_PREDICATE (plotter_tag, x))
    {
      if (SCM_SMOB_DATA (x) != 0)
	return 1;
      else
	return 0;
    }
  else
    return 0;

}

plPlotter *
_scm_to_plotter (SCM x)
{
  assert (_scm_is_plotter (x));

  return (plPlotter *) SCM_SMOB_DATA (x);
}

SCM
_scm_from_plotter (plPlotter * x)
{
  SCM s_plotter;

  assert (x != NULL);

  SCM_NEWSMOB (s_plotter, plotter_tag, x);

  assert (x == (plPlotter *) SCM_SMOB_DATA (s_plotter));

  return (s_plotter);
}

// Plotters are equal if they point to the same C structure
SCM
equalp_plotter (SCM x1, SCM x2)
{
  plPlotter *plotter1, *plotter2;

  plotter1 = (plPlotter *) SCM_SMOB_DATA (x1);
  plotter2 = (plPlotter *) SCM_SMOB_DATA (x2);

  if ((plotter1 == NULL) || (plotter2 == NULL))
    return SCM_BOOL_F;
  else if (plotter1 != plotter2)
    return SCM_BOOL_F;
  else
    return SCM_BOOL_T;
}

SCM
mark_plotter (SCM x)
{
  // No SCMs in the plotter type: nothing to do here.
  return (SCM_BOOL_F);
}

size_t
free_plotter (SCM x)
{
  plPlotter *plotter;

  // assert (SCM_SMOB_PREDICATE (plotter_tag, x));

  plotter = (plPlotter *) SCM_SMOB_DATA (x);
  /* Plotters should already be null if delwin has been called on them */
  if (plotter != NULL)
    {
      pl_deletepl_r (plotter);
      SCM_SET_SMOB_DATA (x, 0);
    }

  return 0;
}

int
print_plotter (SCM x, SCM port, scm_print_state * pstate)
{
  plPlotter *plotter = (plPlotter *) SCM_SMOB_DATA (x);
  char *str;

  assert (SCM_SMOB_PREDICATE (plotter_tag, x));

  scm_puts ("#<plotter ", port);

  if (plotter == 0)
    scm_puts ( "(freed)", port);
  else
    {
      if (asprintf (&str, "%p", (void *) plotter) < 0)
	scm_puts ("???", port);
      else
	scm_puts (str, port);
    }
  scm_puts (">", port);

  // non-zero means success
  return 1;
}

SCM
gupl_is_plotter_p (SCM x)
{
  return scm_from_bool (_scm_is_plotter (x));
}

static ssize_t
port_read (void *cookie, char *buf, size_t siz)
{
  SCM port = PTR2SCM (cookie);

#ifdef GUILE_CHARS_ARE_UCS4
  int c;
  if (siz >= 1)
    {
      c = scm_get_byte_or_eof (port);

      if (c == EOF)
	return 0;
      else
	buf[0] = c;

      return 1;
    }
  else
    return PORT_ERR;
#else
  /* For Guile 1.8.x, we use scm_read_char so we can preserve line
     and column information.  */
  SCM c;
  if (siz >= 1)
    {
      c = scm_read_char (port);

      if (scm_is_true (scm_eof_object_p (c)))
	return 0;
      else
	buf[0] = scm_to_char (c);

      return 1;
    }
  else
    return PORT_ERR;
#endif
}

static ssize_t
port_write (void *cookie, const char *buf, size_t siz)
{
  SCM port = PTR2SCM (cookie);

#ifdef GUILE_CHARS_ARE_UCS4
  if (siz > SSIZE_MAX)
    {
      scm_c_write (port, buf, SSIZE_MAX);
      return SSIZE_MAX;
    }
  else
    {
      scm_c_write (port, buf, siz);
      return siz;
    }
#else
  {
    size_t i;
    SCM chr_as_int;
    SCM chr;

    for (i = 0; i < siz; i++)
      {
	chr_as_int = scm_from_uchar ((unsigned char) buf[i]);
	chr = scm_integer_to_char (chr_as_int);
	scm_write_char (chr, port);
      }
  }

  return siz;
#endif
}

static int
port_seek (void *cookie, off64_t * pos, int whence)
{
  SCM port = PTR2SCM (cookie);
  SCM new_pos;

  new_pos = scm_seek (port, scm_from_int64 (*pos), scm_from_int (whence));
  *pos = scm_to_int64 (new_pos);

  return PORT_OK;
}

static int
port_close (void *cookie)
{
  SCM port = PTR2SCM (cookie);

  scm_close_port (port);

  return PORT_OK;
}


/* Create a new plotter whose output and error are Guile ports */
SCM
gupl_newpl (SCM type, SCM outp, SCM errp, SCM param)
{
  char *c_type;
  FILE *c_outp, *c_errp;
  plPlotter *ret;
  plPlotterParams *c_param;

  SCM_ASSERT (scm_is_string (type), type, SCM_ARG1, "newpl");
  SCM_ASSERT (scm_is_true (scm_output_port_p (outp)), outp, SCM_ARG2,
	      "newpl");
  SCM_ASSERT (scm_is_true (scm_output_port_p (errp)), errp, SCM_ARG3, "newpl");
  SCM_ASSERT (_scm_is_plparams (param), param, SCM_ARG4, "newpl");

  /* Convert the output port to a special stream */
  c_outp = fopencookie (SCM2PTR (outp), "wb", port_funcs);

  /* Don't buffer port here, since the underlying Guile port also has
     port buffering.  Double buffering causes problems.  */

  setvbuf (c_outp, NULL, _IONBF, 0);
  if (c_outp == NULL)
    scm_syserror ("newpl");

  /* Convert the err port to a special stream */
  c_errp = fopencookie (SCM2PTR (errp), "wb", port_funcs);
  if (c_errp == NULL)
    scm_out_of_range ("newpl", errp);
  setvbuf (c_errp, NULL, _IONBF, 0);

  c_type = scm_to_locale_string (type);
  c_param = _scm_to_plparams (param);

  ret = pl_newpl_r (c_type, NULL, c_outp, c_errp, c_param);

  free (c_type);

  if (ret == NULL)
    return SCM_BOOL_F;

  return _scm_from_plotter (ret);
}

static int
_scm_is_exact_integer (SCM x)
{
  return (scm_is_integer (x) && scm_is_false (scm_inexact_p (x)));
}

INT_FUNC_PL_INT_INT_STRING (gupl_alabel_x, "%alabel!", pl_alabel_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_arc_x, "arc!", pl_arc_r, pl_farc_r)
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_arcrel_x, "arcrel!", pl_arcrel_r, pl_farcrel_r)
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_bezier2_x, "bezier2!", pl_bezier2_r, pl_fbezier2_r)
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_bezier2rel_x, "bezier2rel!", pl_bezier2rel_r, pl_fbezier2rel_r)
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_bezier3_x, "bezier3!", pl_bezier3_r, pl_fbezier3_r)
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_bezier3rel_x, "bezier3rel!", pl_bezier3rel_r, pl_fbezier3rel_r)
INT_FUNC_PL_INT_INT_INT (gupl_bgcolor_x, "bgcolor!", pl_bgcolor_r);
INT_FUNC_PL_STRING (gupl_bgcolorname_x, "bgcolorname!", pl_bgcolorname_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL (gupl_box_x, "box!", pl_box_r, pl_fbox_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL (gupl_boxrel_x, "boxrel!", pl_boxrel_r, pl_fboxrel_r);
INT_FUNC_PL_STRING (gupl_capmod_x, "capmod!", pl_capmod_r);
INT_FUNC_PL_VAL_VAL_VAL (gupl_circle_x, "circle!", pl_circle_r, pl_fcircle_r);
INT_FUNC_PL_VAL_VAL_VAL (gupl_circlerel_x, "circlerel!", pl_circlerel_r, pl_fcirclerel_r);
INT_FUNC_PL (gupl_closepl_x, "closepl!", pl_closepl_r);
INT_FUNC_PL_INT_INT_INT (gupl_color_x, "color!", pl_color_r);
INT_FUNC_PL_STRING (gupl_colorname_x, "colorname!", pl_colorname_r);
INT_FUNC_PL_DOUBLE_DOUBLE_DOUBLE_DOUBLE_DOUBLE_DOUBLE (gupl_concat_x, "concat!", pl_fconcat_r);
INT_FUNC_PL_VAL_VAL(gupl_cont_x, "cont!", pl_cont_r, pl_fcont_r);
INT_FUNC_PL_VAL_VAL(gupl_contrel_x, "contrel!", pl_contrel_r, pl_fcontrel_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_ellarc_x, "ellarc!", pl_ellarc_r, pl_fellarc_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_ellarcrel_x, "ellarcrel!", pl_ellarcrel_r, pl_fellarcrel_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL (gupl_ellipse_x, "ellipse!", pl_ellipse_r, pl_fellipse_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL (gupl_ellipserel_x, "ellipserel!", pl_ellipserel_r, pl_fellipserel_r);
INT_FUNC_PL (gupl_endpath_x, "endpath!", pl_endpath_r);
INT_FUNC_PL (gupl_endsubpath_x, "endsubpath!", pl_endsubpath_r);
INT_FUNC_PL (gupl_erase_x, "erase!", pl_erase_r);
INT_FUNC_PL_INT_INT_INT (gupl_fillcolor_x, "fillcolor!", pl_fillcolor_r);
INT_FUNC_PL_STRING (gupl_fillcolorname_x, "fillcolorname!", pl_fillcolorname_r);
INT_FUNC_PL_STRING (gupl_fillmod_x, "fillmod!", pl_fillmod_r);
INT_FUNC_PL_INT(gupl_filltype_x, "filltype!", pl_filltype_r);
INT_FUNC_PL (gupl_flushpl_x, "flushpl!", pl_flushpl_r);
DOUBLE_FUNC_PL_STRING (gupl_fontname_x, "fontname!", pl_ffontname_r);
VAL_FUNC_PL_VAL (gupl_fontsize_x, "fontsize!", pl_fontsize_r, pl_ffontsize_r);
INT_FUNC_PL_STRING (gupl_havecap, "havecap", pl_havecap_r);
INT_FUNC_PL_STRING (gupl_joinmod_x, "joinmod!", pl_joinmod_r);
INT_FUNC_PL_STRING (gupl_label_x, "label!", pl_label_r);
DOUBLE_FUNC_PL_STRING (gupl_labelwidth_x, "labelwidth!", pl_flabelwidth_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL (gupl_line_x, "line!", pl_line_r, pl_fline_r);
INT_FUNC_PL_STRING (gupl_linemod_x, "linemod!", pl_linemod_r);
INT_FUNC_PL_VAL (gupl_linewidth_x, "linewidth!", pl_linewidth_r, pl_flinewidth_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL (gupl_linerel_x, "linerel!", pl_linerel_r, pl_flinerel_r);
INT_FUNC_PL_VAL_VAL_INT_VAL (gupl_marker_x, "marker!", pl_marker_r, pl_fmarker_r);
INT_FUNC_PL_VAL_VAL_INT_VAL (gupl_markerrel_x, "markerrel!", pl_markerrel_r, pl_fmarkerrel_r);
INT_FUNC_PL_DOUBLE (gupl_miterlimit_x, "miterlimit!", pl_fmiterlimit_r);
INT_FUNC_PL_VAL_VAL(gupl_move_x, "move!", pl_move_r, pl_fmove_r);
INT_FUNC_PL_VAL_VAL(gupl_moverel_x, "moverel!", pl_moverel_r, pl_fmoverel_r);
INT_FUNC_PL (gupl_openpl_x, "openpl!", pl_openpl_r);
INT_FUNC_PL_INT (gupl_orientation_x, "orientation!", pl_orientation_r);
INT_FUNC_PL_INT_INT_INT (gupl_pencolor_x, "pencolor!", pl_pencolor_r);
INT_FUNC_PL_STRING (gupl_pencolorname_x, "pencolorname!", pl_pencolorname_r);
INT_FUNC_PL_INT (gupl_pentype_x, "pentype!", pl_pentype_r);
INT_FUNC_PL_VAL_VAL (gupl_point_x, "point!", pl_point_r, pl_fpoint_r);
INT_FUNC_PL_VAL_VAL (gupl_pointrel_x, "pointrel!", pl_pointrel_r, pl_fpointrel_r);
INT_FUNC_PL (gupl_restorestate_x, "restorestate!", pl_restorestate_r);
INT_FUNC_PL_DOUBLE (gupl_rotate_x, "rotate!", pl_frotate_r);
INT_FUNC_PL (gupl_savestate_x, "savestate!", pl_savestate_r);
INT_FUNC_PL_DOUBLE_DOUBLE (gupl_scale_x, "scale!", pl_fscale_r);
INT_FUNC_PL_DOUBLE_DOUBLE_DOUBLE_DOUBLE_DOUBLE_DOUBLE (gupl_setmatrix_x, "setmatrix!", pl_fsetmatrix_r);

SCM
gupl_setplparam_x (SCM s_plparams, SCM s_parameter, SCM s_value)
{
  plPlotterParams *c_plparams;
  char *c_parameter;
  char *c_string_value;
  int c_ret;

  SCM_ASSERT (_scm_is_plparams (s_plparams), s_plparams, SCM_ARG1, "setplparam!");
  SCM_ASSERT (scm_is_string (s_parameter), s_parameter, SCM_ARG2, "setplparam!");
  SCM_ASSERT (scm_is_string (s_value), s_value, SCM_ARG3, "setplparam!");

  c_plparams = _scm_to_plparams (s_plparams);
  c_parameter = scm_to_locale_string (s_parameter);
  c_string_value = scm_to_locale_string (s_value);
  c_ret = pl_setplparam (c_plparams, c_parameter, c_string_value);
  free (c_parameter);
  free (c_string_value);

  return (scm_from_int (c_ret));
}


INT_FUNC_PL_VAL_VAL_VAL_VAL (gupl_space_x, "space!", pl_space_r, pl_fspace_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_space2_x, "space2!", pl_space2_r, pl_fspace2_r)
VAL_FUNC_PL_VAL (gupl_textangle_x, "textangle!", pl_textangle_r, pl_ftextangle_r);
INT_FUNC_PL_DOUBLE_DOUBLE (gupl_translate_x, "translate!", pl_ftranslate_r);


void
libguile_plotutils_LTX_gupl_plot_init (void)
{
  gupl_plot_init ();
}

void
gupl_plot_init (void)
{
  static int first = 1;

  if (first)
    {
      port_funcs.read = port_read;
      port_funcs.write = port_write;
      port_funcs.seek = port_seek;
      port_funcs.close = port_close;

      plparams_tag = scm_make_smob_type ("plparams", sizeof (plPlotterParams *));
      scm_set_smob_mark (plparams_tag, mark_plparams);
      scm_set_smob_free (plparams_tag, free_plparams);
      scm_set_smob_print (plparams_tag, print_plparams);
      scm_set_smob_equalp (plparams_tag, equalp_plparams);
      scm_c_define_gsubr ("plparams?", 1, 0, 0, gupl_is_plparams_p);
      scm_c_define_gsubr ("newplparams", 0, 0, 0, gupl_newplparams);

      plotter_tag = scm_make_smob_type ("plotter", sizeof (plPlotter *));
      scm_set_smob_mark (plotter_tag, mark_plotter);
      scm_set_smob_free (plotter_tag, free_plotter);
      scm_set_smob_print (plotter_tag, print_plotter);
      scm_set_smob_equalp (plotter_tag, equalp_plotter);
      scm_c_define_gsubr ("plotter?", 1, 0, 0, gupl_is_plotter_p);
      scm_c_define_gsubr ("newpl", 4, 0, 0, gupl_newpl);

      scm_c_define_gsubr ("%alabel!", 4, 0, 0, gupl_alabel_x);
      scm_c_define_gsubr ("arc!", 7, 0, 0, gupl_arc_x);
      scm_c_define_gsubr ("arcrel!", 7, 0, 0, gupl_arcrel_x);
      scm_c_define_gsubr ("bezier2!", 7, 0, 0, gupl_bezier2_x);
      scm_c_define_gsubr ("bezier2rel!", 7, 0, 0, gupl_bezier2rel_x);
      scm_c_define_gsubr ("bezier3!", 9, 0, 0, gupl_bezier3_x);
      scm_c_define_gsubr ("bezier3rel!", 9, 0, 0, gupl_bezier3rel_x);
      scm_c_define_gsubr ("bgcolor!", 4, 0, 0, gupl_bgcolor_x);
      scm_c_define_gsubr ("bgcolorname!", 2, 0, 0, gupl_bgcolorname_x);
      scm_c_define_gsubr ("box!", 5, 0, 0, gupl_box_x);
      scm_c_define_gsubr ("boxrel!", 5, 0, 0, gupl_boxrel_x);
      scm_c_define_gsubr ("capmod!", 2, 0, 0, gupl_capmod_x);
      scm_c_define_gsubr ("circle!", 4, 0, 0, gupl_circle_x);
      scm_c_define_gsubr ("circlerel!", 4, 0, 0, gupl_circlerel_x);
      scm_c_define_gsubr ("closepl!", 1, 0, 0, gupl_closepl_x);
      scm_c_define_gsubr ("color!", 4, 0, 0, gupl_color_x);
      scm_c_define_gsubr ("colorname!", 2, 0, 0, gupl_colorname_x);
      scm_c_define_gsubr ("concat!", 7, 0, 0, gupl_concat_x);
      scm_c_define_gsubr ("cont!", 3, 0, 0, gupl_cont_x);
      scm_c_define_gsubr ("contrel!", 3, 0, 0, gupl_contrel_x);
      scm_c_define_gsubr ("ellarc!", 7, 0, 0, gupl_ellarc_x);
      scm_c_define_gsubr ("ellarcrel!", 7, 0, 0, gupl_ellarcrel_x);
      scm_c_define_gsubr ("ellipse!", 6, 0, 0, gupl_ellipse_x);
      scm_c_define_gsubr ("ellipserel!", 6, 0, 0, gupl_ellipserel_x);
      scm_c_define_gsubr ("endpath!", 1, 0, 0, gupl_endpath_x);
      scm_c_define_gsubr ("endsubpath!", 1, 0, 0, gupl_endsubpath_x);
      scm_c_define_gsubr ("erase!", 1, 0, 0, gupl_erase_x);
      scm_c_define_gsubr ("fillcolor!", 4, 0, 0, gupl_fillcolor_x);
      scm_c_define_gsubr ("fillcolorname!", 2, 0, 0, gupl_fillcolorname_x);
      scm_c_define_gsubr ("fillmod!", 2, 0, 0, gupl_fillmod_x);
      scm_c_define_gsubr ("filltype!", 2, 0, 0, gupl_filltype_x);
      scm_c_define_gsubr ("flushpl!", 1, 0, 0, gupl_flushpl_x);
      scm_c_define_gsubr ("fontname!", 2, 0, 0, gupl_fontname_x);
      scm_c_define_gsubr ("fontsize!", 2, 0, 0, gupl_fontsize_x);
      scm_c_define_gsubr ("havecap", 2, 0, 0, gupl_havecap);
      scm_c_define_gsubr ("joinmod!", 2, 0, 0, gupl_joinmod_x);
      scm_c_define_gsubr ("label!", 2, 0, 0, gupl_label_x);
      scm_c_define_gsubr ("labelwidth!", 2, 0, 0, gupl_labelwidth_x);
      scm_c_define_gsubr ("line!", 5, 0, 0, gupl_line_x);
      scm_c_define_gsubr ("linemod!", 2, 0, 0, gupl_linemod_x);
      scm_c_define_gsubr ("linerel!", 5, 0, 0, gupl_linerel_x);
      scm_c_define_gsubr ("linewidth!", 2, 0, 0, gupl_linewidth_x);
      scm_c_define_gsubr ("marker!", 5, 0, 0, gupl_marker_x);
      scm_c_define_gsubr ("markerrel!", 5, 0, 0, gupl_markerrel_x);
      scm_c_define_gsubr ("miterlimit!", 2, 0, 0, gupl_miterlimit_x);
      scm_c_define_gsubr ("move!", 3, 0, 0, gupl_move_x);
      scm_c_define_gsubr ("moverel!", 3, 0, 0, gupl_moverel_x);
      scm_c_define_gsubr ("openpl!", 1, 0, 0, gupl_openpl_x);
      scm_c_define_gsubr ("orientation!", 2, 0, 0, gupl_orientation_x);
      scm_c_define_gsubr ("pencolor!", 4, 0, 0, gupl_pencolor_x);
      scm_c_define_gsubr ("pencolorname!", 2, 0, 0, gupl_pencolorname_x);
      scm_c_define_gsubr ("pentype!", 2, 0, 0, gupl_pentype_x);
      scm_c_define_gsubr ("point!", 3, 0, 0, gupl_point_x);
      scm_c_define_gsubr ("pointrel!", 3, 0, 0, gupl_pointrel_x);
      scm_c_define_gsubr ("restorestate!", 1, 0, 0, gupl_restorestate_x);
      scm_c_define_gsubr ("rotate!", 2, 0, 0, gupl_rotate_x);
      scm_c_define_gsubr ("savestate!", 1, 0, 0, gupl_savestate_x);
      scm_c_define_gsubr ("scale!", 3, 0, 0, gupl_scale_x);
      scm_c_define_gsubr ("setmatrix!", 7, 0, 0, gupl_setmatrix_x);
      scm_c_define_gsubr ("setplparam!", 3, 0, 0, gupl_setplparam_x);
      scm_c_define_gsubr ("space!", 5, 0, 0, gupl_space_x);
      scm_c_define_gsubr ("space2!", 7, 0, 0, gupl_space2_x);
      scm_c_define_gsubr ("textangle!", 2, 0, 0, gupl_textangle_x);
      scm_c_define_gsubr ("translate!", 3, 0, 0, gupl_translate_x);

      first = 0;
    }
}
