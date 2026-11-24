#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include <libguile.h>

#include "fontlist.h"
#include "guile_plot.h"
#include "plot.h"

#define PORT_ERR (-1)
#define PORT_OK (0)

static scm_t_bits plotter_tag;
static scm_t_bits plparams_tag;

/* A plotter smob owns its libplot Plotter and the two stdio streams the
   Plotter writes to, so that all three can be released together.  */
typedef struct
{
  plPlotter *plotter;
  FILE *outp;
  FILE *errp;
  /* The Guile ports the streams were duplicated from.  libplot writes to
     the duplicated file descriptors directly, bypassing these ports'
     buffers, so the ports are flushed before every libplot call to keep
     Scheme output and plotter output in order.  */
  SCM s_outp;
  SCM s_errp;
} gupl_plotter_t;

/* Write out anything buffered in PORT, unless the port has been closed
   (its file descriptor was duplicated, so the plotter can still write). */
static void
flush_port_if_open (SCM port)
{
  if (scm_is_false (scm_port_closed_p (port)))
    scm_force_output (port);
}

static void
flush_plotter_ports (gupl_plotter_t *plotter)
{
  flush_port_if_open (plotter->s_outp);
  flush_port_if_open (plotter->s_errp);
}

static int _scm_is_plparams (SCM x);
static plPlotterParams *_scm_to_plparams (SCM x);
static SCM _scm_from_plparams (plPlotterParams *x);
static SCM equalp_plparams (SCM x1, SCM x2);
static SCM mark_plparams (SCM x);
static size_t free_plparams (SCM x);
static int print_plparams (SCM x, SCM port, scm_print_state *pstate);

static int _scm_is_plotter (SCM x);
static plPlotter *_scm_to_plotter (SCM x);
static SCM _scm_from_plotter (gupl_plotter_t *x);
static SCM equalp_plotter (SCM x1, SCM x2);
static SCM mark_plotter (SCM x);
static size_t free_plotter (SCM x);
static int print_plotter (SCM x, SCM port, scm_print_state *pstate);

static int _scm_is_exact_integer (SCM x);
static int _scm_to_int_arg (SCM x);

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

  return (plPlotterParams *)SCM_SMOB_DATA (x);
}

SCM
_scm_from_plparams (plPlotterParams *x)
{
  SCM s_plparams;

  assert (x != NULL);

  SCM_NEWSMOB (s_plparams, plparams_tag, x);

  assert (x == (plPlotterParams *)SCM_SMOB_DATA (s_plparams));

  return (s_plparams);
}

SCM
equalp_plparams (SCM x1, SCM x2)
{
  plPlotterParams *plparams1, *plparams2;

  plparams1 = (plPlotterParams *)SCM_SMOB_DATA (x1);
  plparams2 = (plPlotterParams *)SCM_SMOB_DATA (x2);

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

  plparams = (plPlotterParams *)SCM_SMOB_DATA (x);
  if (plparams != NULL)
    {
      pl_deleteplparams (plparams);
      SCM_SET_SMOB_DATA (x, 0);
    }

  return 0;
}

int
print_plparams (SCM x, SCM port, scm_print_state *pstate)
{
  plPlotterParams *plparams = (plPlotterParams *)SCM_SMOB_DATA (x);
  char *str;

  assert (SCM_SMOB_PREDICATE (plparams_tag, x));

  scm_puts ("#<plparams ", port);

  if (plparams == 0)
    scm_puts ("(freed)", port);
  else
    {
      if (asprintf (&str, "%p", (void *)plparams) < 0)
        scm_puts ("???", port);
      else
        {
          scm_puts (str, port);
          free (str);
        }
    }
  scm_puts (">", port);

  // non-zero means success
  return 1;
}

SCM
gupl_is_plparams_p (SCM x)
{
  return scm_from_bool (_scm_is_plparams (x));
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
  gupl_plotter_t *plotter;

  assert (_scm_is_plotter (x));

  /* Every plotting procedure gets its Plotter here, just before calling
     libplot, so this is where pending Scheme output is written out.  */
  plotter = (gupl_plotter_t *)SCM_SMOB_DATA (x);
  flush_plotter_ports (plotter);
  return plotter->plotter;
}

SCM
_scm_from_plotter (gupl_plotter_t *x)
{
  SCM s_plotter;

  assert (x != NULL);

  SCM_NEWSMOB (s_plotter, plotter_tag, x);

  assert (x == (gupl_plotter_t *)SCM_SMOB_DATA (s_plotter));

  return (s_plotter);
}

// Plotters are equal if they point to the same C structure
SCM
equalp_plotter (SCM x1, SCM x2)
{
  gupl_plotter_t *plotter1, *plotter2;

  plotter1 = (gupl_plotter_t *)SCM_SMOB_DATA (x1);
  plotter2 = (gupl_plotter_t *)SCM_SMOB_DATA (x2);

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
  gupl_plotter_t *plotter = (gupl_plotter_t *)SCM_SMOB_DATA (x);

  if (plotter == NULL)
    return SCM_BOOL_F;
  scm_gc_mark (plotter->s_outp);
  return plotter->s_errp;
}

/* Delete the libplot Plotter held by smob X and close its streams, if
   that hasn't been done already.  Returns pl_deletepl_r's result, or 0 if
   the plotter was already deleted.  */
static int
release_plotter (SCM x)
{
  gupl_plotter_t *plotter = (gupl_plotter_t *)SCM_SMOB_DATA (x);
  int ret;

  if (plotter == NULL)
    return 0;

  /* Clear the smob first, so the plotter can't be used, or released
     again, if anything below fails.  */
  SCM_SET_SMOB_DATA (x, 0);

  /* delete the Plotter first: it may flush output to the streams */
  ret = pl_deletepl_r (plotter->plotter);
  fclose (plotter->outp);
  fclose (plotter->errp);
  free (plotter);
  return ret;
}

size_t
free_plotter (SCM x)
{
  release_plotter (x);
  return 0;
}

SCM
gupl_deletepl_x (SCM s_plotter)
{
  gupl_plotter_t *plotter;

  SCM_ASSERT (SCM_SMOB_PREDICATE (plotter_tag, s_plotter), s_plotter,
              SCM_ARG1, "deletepl!");
  /* Deleting may write output, so flush first.  (The garbage collector's
     free function can't do this: it must not run port code.)  */
  plotter = (gupl_plotter_t *)SCM_SMOB_DATA (s_plotter);
  if (plotter != NULL)
    flush_plotter_ports (plotter);
  return scm_from_int (release_plotter (s_plotter));
}

int
print_plotter (SCM x, SCM port, scm_print_state *pstate)
{
  gupl_plotter_t *data = (gupl_plotter_t *)SCM_SMOB_DATA (x);
  plPlotter *plotter = data ? data->plotter : NULL;
  char *str;

  assert (SCM_SMOB_PREDICATE (plotter_tag, x));

  scm_puts ("#<plotter ", port);

  if (plotter == 0)
    scm_puts ("(freed)", port);
  else
    {
      if (asprintf (&str, "%p", (void *)plotter) < 0)
        scm_puts ("???", port);
      else
        {
          scm_puts (str, port);
          free (str);
        }
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

/* Create a new plotter whose output and error are Guile ports */
SCM
gupl_newpl (SCM type, SCM outp, SCM errp, SCM param)
{
  char *c_type;
  plPlotter *ret;
  plPlotterParams *c_param;
  gupl_plotter_t *data;
  bool outp_opened = false;
  bool errp_opened = false;

  SCM_ASSERT (scm_is_string (type), type, SCM_ARG1, "newpl");
  SCM_ASSERT (scm_is_true (scm_output_port_p (outp)), outp, SCM_ARG2, "newpl");
  SCM_ASSERT (scm_is_true (scm_output_port_p (errp)), errp, SCM_ARG3, "newpl");
  SCM_ASSERT (_scm_is_plparams (param), param, SCM_ARG4, "newpl");

  /* Write out anything already buffered in the ports, so that it comes
     before the plotter's output.  */
  flush_port_if_open (outp);
  flush_port_if_open (errp);

  /* Do everything that can throw before opening any streams.  */
  int c_outp_fileno_orig = scm_to_int (scm_fileno (outp));
  int c_errp_fileno_orig = scm_to_int (scm_fileno (errp));
  c_type = scm_to_locale_string (type);

  int c_outp_fileno = dup (c_outp_fileno_orig);
  FILE *c_outp;

  if (c_outp_fileno < 0)
    {
      free (c_type);
      scm_misc_error ("newpl", "failed to duplicate output port", SCM_EOL);
    }

  c_outp = fdopen (c_outp_fileno, "w");
  if (c_outp == NULL)
    {
      close (c_outp_fileno);
      free (c_type);
      scm_misc_error ("newpl", "failed to open duplicated output port",
                      SCM_EOL);
    }
  outp_opened = true;

  int c_errp_fileno = dup (c_errp_fileno_orig);
  FILE *c_errp;

  if (c_errp_fileno < 0)
    {
      fclose (c_outp);
      free (c_type);
      scm_misc_error ("newpl", "failed to duplicate error port", SCM_EOL);
    }

  c_errp = fdopen (c_errp_fileno, "w");
  if (c_errp == NULL)
    {
      close (c_errp_fileno);
      fclose (c_outp);
      free (c_type);
      scm_misc_error ("newpl", "failed to open duplicated error port",
                      SCM_EOL);
    }
  errp_opened = true;

  /* Don't buffer port here, since the underlying Guile port also has
     port buffering.  Double buffering causes problems.  */

  if (c_outp)
    setvbuf (c_outp, NULL, _IONBF, 0);
  if (c_errp)
    setvbuf (c_errp, NULL, _IONBF, 0);

  c_param = _scm_to_plparams (param);

  ret = pl_newpl_r (c_type, NULL, c_outp, c_errp, c_param);

  free (c_type);

  if (ret == NULL)
    {
      if (outp_opened)
        fclose (c_outp);
      if (errp_opened)
        fclose (c_errp);
      return SCM_BOOL_F;
    }

  data = scm_malloc (sizeof (gupl_plotter_t));
  data->plotter = ret;
  data->outp = c_outp;
  data->errp = c_errp;
  data->s_outp = outp;
  data->s_errp = errp;
  return _scm_from_plotter (data);
}

/* foreach_plotter_font callback: push NAME onto the list *DATA.  */
static void
cons_font_name (const char *name, void *data)
{
  SCM *list = (SCM *)data;

  *list = scm_cons (scm_from_latin1_string (name), *list);
}

/* (list-fonts output-format-or-plotter): the names of the fonts that a
   Plotter of the given output format (a string such as "svg"), or the
   given Plotter, supports.  */
SCM
gupl_list_fonts (SCM s_target)
{
  plPlotter *plotter;
  bool temporary = false;
  SCM list = SCM_EOL;

  if (_scm_is_plotter (s_target))
    plotter = _scm_to_plotter (s_target);
  else
    {
      plPlotterParams *params;
      char *c_type;

      SCM_ASSERT (scm_is_string (s_target), s_target, SCM_ARG1, "list-fonts");
      c_type = scm_to_locale_string (s_target);
      /* A Plotter just for asking: with no output or error stream, it
         writes nothing, even when deleted.  */
      params = pl_newplparams ();
      plotter = pl_newpl_r (c_type, NULL, NULL, NULL, params);
      pl_deleteplparams (params);
      free (c_type);
      if (plotter == NULL)
        scm_misc_error ("list-fonts", "unknown output format: ~S",
                        scm_list_1 (s_target));
      temporary = true;
    }

  int count = foreach_plotter_font (plotter, cons_font_name, &list);
  if (temporary)
    pl_deletepl_r (plotter);
  if (count < 0)
    scm_misc_error ("list-fonts",
                    "cannot read this libplot's font tables (unrecognized "
                    "layout)",
                    SCM_EOL);
  return scm_reverse_x (list, SCM_EOL);
}

static int
_scm_is_exact_integer (SCM x)
{
  /* Coordinates that are exact integers go to libplot's int functions,
     but only if they fit in an int; larger ones use the float functions. */
  return scm_is_signed_integer (x, INT_MIN, INT_MAX);
}

/* Convert an argument that must be an integer (checked with
   scm_is_integer), accepting integer-valued reals such as 2.0.  */
static int
_scm_to_int_arg (SCM x)
{
  return scm_to_int (scm_is_exact (x) ? x : scm_inexact_to_exact (x));
}

INT_FUNC_PL_INT_INT_STRING (gupl_alabel_x, "%alabel!", pl_alabel_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_arc_x, "arc!", pl_arc_r, pl_farc_r)
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_arcrel_x, "arcrel!", pl_arcrel_r,
                                     pl_farcrel_r)
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_bezier2_x, "bezier2!", pl_bezier2_r,
                                     pl_fbezier2_r)
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_bezier2rel_x, "bezier2rel!",
                                     pl_bezier2rel_r, pl_fbezier2rel_r)
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_bezier3_x, "bezier3!",
                                             pl_bezier3_r, pl_fbezier3_r)
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_bezier3rel_x, "bezier3rel!",
                                             pl_bezier3rel_r, pl_fbezier3rel_r)
INT_FUNC_PL_INT_INT_INT (gupl_bgcolor_x, "bgcolor!", pl_bgcolor_r);
INT_FUNC_PL_STRING (gupl_bgcolorname_x, "bgcolorname!", pl_bgcolorname_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL (gupl_box_x, "box!", pl_box_r, pl_fbox_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL (gupl_boxrel_x, "boxrel!", pl_boxrel_r,
                             pl_fboxrel_r);
INT_FUNC_PL_STRING (gupl_capmod_x, "capmod!", pl_capmod_r);
INT_FUNC_PL_VAL_VAL_VAL (gupl_circle_x, "circle!", pl_circle_r, pl_fcircle_r);
INT_FUNC_PL_VAL_VAL_VAL (gupl_circlerel_x, "circlerel!", pl_circlerel_r,
                         pl_fcirclerel_r);
INT_FUNC_PL (gupl_closepath_x, "closepath!", pl_closepath_r);
INT_FUNC_PL (gupl_closepl_x, "closepl!", pl_closepl_r);
INT_FUNC_PL_INT_INT_INT (gupl_color_x, "color!", pl_color_r);
INT_FUNC_PL_STRING (gupl_colorname_x, "colorname!", pl_colorname_r);
INT_FUNC_PL_DOUBLE_DOUBLE_DOUBLE_DOUBLE_DOUBLE_DOUBLE (gupl_concat_x,
                                                       "concat!",
                                                       pl_fconcat_r);
INT_FUNC_PL_VAL_VAL (gupl_cont_x, "cont!", pl_cont_r, pl_fcont_r);
INT_FUNC_PL_VAL_VAL (gupl_contrel_x, "contrel!", pl_contrel_r, pl_fcontrel_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_ellarc_x, "ellarc!", pl_ellarc_r,
                                     pl_fellarc_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_ellarcrel_x, "ellarcrel!",
                                     pl_ellarcrel_r, pl_fellarcrel_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL (gupl_ellipse_x, "ellipse!", pl_ellipse_r,
                                 pl_fellipse_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL (gupl_ellipserel_x, "ellipserel!",
                                 pl_ellipserel_r, pl_fellipserel_r);
INT_FUNC_PL (gupl_endpath_x, "endpath!", pl_endpath_r);
INT_FUNC_PL (gupl_endsubpath_x, "endsubpath!", pl_endsubpath_r);
INT_FUNC_PL (gupl_erase_x, "erase!", pl_erase_r);
INT_FUNC_PL_INT_INT_INT (gupl_fillcolor_x, "fillcolor!", pl_fillcolor_r);
INT_FUNC_PL_STRING (gupl_fillcolorname_x, "fillcolorname!",
                    pl_fillcolorname_r);
INT_FUNC_PL_STRING (gupl_fillmod_x, "fillmod!", pl_fillmod_r);
INT_FUNC_PL_INT (gupl_filltype_x, "filltype!", pl_filltype_r);
INT_FUNC_PL (gupl_flushpl_x, "flushpl!", pl_flushpl_r);
DOUBLE_FUNC_PL_STRING (gupl_fontname_x, "fontname!", pl_ffontname_r);
VAL_FUNC_PL_VAL (gupl_fontsize_x, "fontsize!", pl_fontsize_r, pl_ffontsize_r);
INT_FUNC_PL_STRING (gupl_havecap, "havecap", pl_havecap_r);
INT_FUNC_PL_STRING (gupl_joinmod_x, "joinmod!", pl_joinmod_r);
INT_FUNC_PL_STRING (gupl_label_x, "label!", pl_label_r);
DOUBLE_FUNC_PL_STRING (gupl_labelwidth_x, "labelwidth!", pl_flabelwidth_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL (gupl_line_x, "line!", pl_line_r, pl_fline_r);
INT_FUNC_PL_STRING (gupl_linemod_x, "linemod!", pl_linemod_r);

/* (linedash! plotter dashes [offset]): DASHES is a list of real numbers
   and OFFSET (default 0) a real number.  As with the other procedures,
   libplot's integer version is used when every value is an exact integer
   that fits in an int, and the floating-point version otherwise.  */
SCM
gupl_linedash_x (SCM s_plotter, SCM s_dashes, SCM s_offset)
{
  plPlotter *c_plotter;
  long n, i;
  bool all_int;
  SCM rest;
  int c_ret;

  SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, "linedash!");
  n = scm_ilength (s_dashes);
  SCM_ASSERT (n >= 0, s_dashes, SCM_ARG2, "linedash!");
  if (SCM_UNBNDP (s_offset))
    s_offset = scm_from_int (0);
  SCM_ASSERT (scm_is_real (s_offset), s_offset, SCM_ARG3, "linedash!");

  /* Check every element before allocating, so nothing below can throw. */
  all_int = scm_is_signed_integer (s_offset, INT_MIN, INT_MAX);
  for (rest = s_dashes; !scm_is_null (rest); rest = scm_cdr (rest))
    {
      SCM x = scm_car (rest);
      SCM_ASSERT (scm_is_real (x), s_dashes, SCM_ARG2, "linedash!");
      if (!scm_is_signed_integer (x, INT_MIN, INT_MAX))
        all_int = false;
    }

  c_plotter = _scm_to_plotter (s_plotter);
  if (all_int)
    {
      int *dashes = scm_malloc ((n > 0 ? n : 1) * sizeof (int));
      for (i = 0, rest = s_dashes; i < n; i++, rest = scm_cdr (rest))
        dashes[i] = scm_to_int (scm_car (rest));
      c_ret = pl_linedash_r (c_plotter, (int)n, dashes,
                             scm_to_int (s_offset));
      free (dashes);
    }
  else
    {
      double *dashes = scm_malloc ((n > 0 ? n : 1) * sizeof (double));
      for (i = 0, rest = s_dashes; i < n; i++, rest = scm_cdr (rest))
        dashes[i] = scm_to_double (scm_car (rest));
      c_ret = pl_flinedash_r (c_plotter, (int)n, dashes,
                              scm_to_double (s_offset));
      free (dashes);
    }
  return scm_from_int (c_ret);
}
INT_FUNC_PL_VAL (gupl_linewidth_x, "linewidth!", pl_linewidth_r,
                 pl_flinewidth_r);
INT_FUNC_PL_VAL_VAL_VAL_VAL (gupl_linerel_x, "linerel!", pl_linerel_r,
                             pl_flinerel_r);
INT_FUNC_PL_VAL_VAL_INT_VAL (gupl_marker_x, "marker!", pl_marker_r,
                             pl_fmarker_r);
INT_FUNC_PL_VAL_VAL_INT_VAL (gupl_markerrel_x, "markerrel!", pl_markerrel_r,
                             pl_fmarkerrel_r);
INT_FUNC_PL_DOUBLE (gupl_miterlimit_x, "miterlimit!", pl_fmiterlimit_r);
INT_FUNC_PL_VAL_VAL (gupl_move_x, "move!", pl_move_r, pl_fmove_r);
INT_FUNC_PL_VAL_VAL (gupl_moverel_x, "moverel!", pl_moverel_r, pl_fmoverel_r);
INT_FUNC_PL (gupl_openpl_x, "openpl!", pl_openpl_r);
INT_FUNC_PL_INT (gupl_orientation_x, "orientation!", pl_orientation_r);
INT_FUNC_PL_INT_INT_INT (gupl_pencolor_x, "pencolor!", pl_pencolor_r);
INT_FUNC_PL_STRING (gupl_pencolorname_x, "pencolorname!", pl_pencolorname_r);
INT_FUNC_PL_INT (gupl_pentype_x, "pentype!", pl_pentype_r);
INT_FUNC_PL_VAL_VAL (gupl_point_x, "point!", pl_point_r, pl_fpoint_r);
INT_FUNC_PL_VAL_VAL (gupl_pointrel_x, "pointrel!", pl_pointrel_r,
                     pl_fpointrel_r);
INT_FUNC_PL (gupl_restorestate_x, "restorestate!", pl_restorestate_r);
INT_FUNC_PL_DOUBLE (gupl_rotate_x, "rotate!", pl_frotate_r);
INT_FUNC_PL (gupl_savestate_x, "savestate!", pl_savestate_r);
INT_FUNC_PL_DOUBLE_DOUBLE (gupl_scale_x, "scale!", pl_fscale_r);
INT_FUNC_PL_DOUBLE_DOUBLE_DOUBLE_DOUBLE_DOUBLE_DOUBLE (gupl_setmatrix_x,
                                                       "setmatrix!",
                                                       pl_fsetmatrix_r);

SCM
gupl_setplparam_x (SCM s_plparams, SCM s_parameter, SCM s_value)
{
  plPlotterParams *c_plparams;
  char *c_parameter;
  char *c_string_value;
  int c_ret;

  SCM_ASSERT (_scm_is_plparams (s_plparams), s_plparams, SCM_ARG1,
              "setplparam!");
  SCM_ASSERT (scm_is_string (s_parameter), s_parameter, SCM_ARG2,
              "setplparam!");
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
INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL (gupl_space2_x, "space2!", pl_space2_r,
                                     pl_fspace2_r)
VAL_FUNC_PL_VAL (gupl_textangle_x, "textangle!", pl_textangle_r,
                 pl_ftextangle_r);
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
      plparams_tag
          = scm_make_smob_type ("plparams", sizeof (plPlotterParams *));
      scm_set_smob_mark (plparams_tag, mark_plparams);
      scm_set_smob_free (plparams_tag, free_plparams);
      scm_set_smob_print (plparams_tag, print_plparams);
      scm_set_smob_equalp (plparams_tag, equalp_plparams);
      scm_c_define_gsubr ("plparams?", 1, 0, 0, gupl_is_plparams_p);
      scm_c_define_gsubr ("newplparams", 0, 0, 0, gupl_newplparams);

      plotter_tag = scm_make_smob_type ("plotter", sizeof (gupl_plotter_t *));
      scm_set_smob_mark (plotter_tag, mark_plotter);
      scm_set_smob_free (plotter_tag, free_plotter);
      scm_set_smob_print (plotter_tag, print_plotter);
      scm_set_smob_equalp (plotter_tag, equalp_plotter);
      scm_c_define_gsubr ("plotter?", 1, 0, 0, gupl_is_plotter_p);
      scm_c_define_gsubr ("newpl", 4, 0, 0, gupl_newpl);
      scm_c_define_gsubr ("deletepl!", 1, 0, 0, gupl_deletepl_x);
      scm_c_define_gsubr ("list-fonts", 1, 0, 0, gupl_list_fonts);

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
      scm_c_define_gsubr ("closepath!", 1, 0, 0, gupl_closepath_x);
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
      scm_c_define_gsubr ("linedash!", 2, 1, 0, gupl_linedash_x);
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
