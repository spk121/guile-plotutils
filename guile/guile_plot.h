#ifndef GUPL_TYPES_H
#define GUPL_TYPES_H

#include <libguile.h>
#include <plot.h>
#include "visibility.h"

/* TYPES */

/* plot parameters type*/
GUPL_API SCM gupl_is_plparams_p (SCM x);
GUPL_API SCM gupl_newplparams (void);

/* plotter type */
GUPL_API SCM gupl_is_plotter_p (SCM x);
GUPL_API SCM gupl_newpl (SCM type, SCM outp, SCM errp, SCM plparams);

/* initialization */
GUPL_API void gupl_plot_init (void);

/* functions */
GUPL_API SCM gupl_alabel_x (SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_arc_x (SCM, SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_arcrel_x (SCM, SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_bezier2_x (SCM, SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_bezier2rel_x (SCM, SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_bezier3_x (SCM, SCM, SCM, SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_bezier3rel_x (SCM, SCM, SCM, SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_bgcolor_x (SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_bgcolorname_x (SCM, SCM);
GUPL_API SCM gupl_box_x (SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_boxrel_x (SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_capmod_x (SCM, SCM);
GUPL_API SCM gupl_circle_x (SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_circlerel_x (SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_closepl_x (SCM);
GUPL_API SCM gupl_color_x (SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_colorname_x (SCM, SCM);
GUPL_API SCM gupl_concat_x (SCM, SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_cont_x (SCM, SCM, SCM);
GUPL_API SCM gupl_contrel_x (SCM, SCM, SCM);
GUPL_API SCM gupl_ellarc_x (SCM, SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_ellarcrel_x (SCM, SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_ellipse_x (SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_ellipserel_x (SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_endpath_x (SCM);
GUPL_API SCM gupl_endsubpath_x (SCM);
GUPL_API SCM gupl_erase_x (SCM);
GUPL_API SCM gupl_fillcolor_x (SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_fillcolorname_x (SCM, SCM);
GUPL_API SCM gupl_fillmod_x (SCM, SCM);
GUPL_API SCM gupl_filltype_x (SCM, SCM);
GUPL_API SCM gupl_flushpl_x (SCM);
GUPL_API SCM gupl_fontname_x (SCM, SCM);
GUPL_API SCM gupl_fontsize_x (SCM, SCM);
GUPL_API SCM gupl_havecap (SCM, SCM);
GUPL_API SCM gupl_joinmod_x (SCM, SCM);
GUPL_API SCM gupl_label_x (SCM, SCM);
GUPL_API SCM gupl_labelwidth_x (SCM, SCM);
GUPL_API SCM gupl_line_x (SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_linemod_x (SCM, SCM);
GUPL_API SCM gupl_linewidth_x (SCM, SCM);
GUPL_API SCM gupl_linerel_x (SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_marker_x (SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_markerrel_x (SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_miterlimit_x (SCM, SCM);
GUPL_API SCM gupl_move_x (SCM, SCM, SCM);
GUPL_API SCM gupl_moverel_x (SCM, SCM, SCM);
GUPL_API SCM gupl_openpl_x (SCM);
GUPL_API SCM gupl_orientation_x (SCM, SCM);
GUPL_API SCM gupl_pencolor_x (SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_pencolorname_x (SCM, SCM);
GUPL_API SCM gupl_pentype_x (SCM, SCM);
GUPL_API SCM gupl_point_x (SCM, SCM, SCM);
GUPL_API SCM gupl_pointrel_x (SCM, SCM, SCM);
GUPL_API SCM gupl_restorestate_x (SCM);
GUPL_API SCM gupl_rotate_x (SCM, SCM);
GUPL_API SCM gupl_savestate_x (SCM);
GUPL_API SCM gupl_scale_x (SCM, SCM, SCM);
GUPL_API SCM gupl_setmatrix_x (SCM, SCM, SCM, SCM, SCM, SCM, SCM);
GUPL_API SCM gupl_setplparam_x (SCM, SCM, SCM);

/* templates */
#define INT_FUNC_PL(CNAME, SNAME, FUNC)					\
  SCM									\
  CNAME (SCM s_plotter)							\
  {									\
    plPlotter *c_plotter;						\
    									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    return scm_from_int (FUNC (c_plotter));				\
  }

#define INT_FUNC_PL_INT_INT_INT(CNAME, SNAME, FUNC)			\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0, SCM s_x1, SCM s_x2)			\
  {									\
    plPlotter *c_plotter;						\
    int c_x0, c_x1, c_x2;						\
    									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_integer (s_x0), s_x0, SCM_ARG2, SNAME);		\
    SCM_ASSERT (scm_is_integer (s_x1), s_x1, SCM_ARG3, SNAME);		\
    SCM_ASSERT (scm_is_integer (s_x2), s_x2, SCM_ARG4, SNAME);		\
    									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    c_x0 = scm_to_int (s_x0);						\
    c_x1 = scm_to_int (s_x1);						\
    c_x2 = scm_to_int (s_x2);						\
    return scm_from_int (FUNC (c_plotter, c_x0, c_x1, c_x2));		\
  }

#define INT_FUNC_PL_INT_INT_STRING(CNAME, SNAME, FUNC)			\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0, SCM s_x1, SCM s_x2)			\
  {									\
    plPlotter *c_plotter;						\
    int c_x0, c_x1;							\
    char *c_x2;								\
    int c_ret;								\
    									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_integer (s_x0), s_x0, SCM_ARG2, SNAME);		\
    SCM_ASSERT (scm_is_integer (s_x1), s_x1, SCM_ARG3, SNAME);		\
    SCM_ASSERT (scm_is_string (s_x2), s_x2, SCM_ARG4, SNAME);		\
    									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    c_x0 = scm_to_int (s_x0);						\
    c_x1 = scm_to_int (s_x1);						\
    c_x2 = scm_to_locale_string (s_x2);					\
    c_ret = FUNC (c_plotter, c_x0, c_x1, c_x2);				\
    free (c_x2);							\
    return scm_from_int (c_ret);					\
  }


#define INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL(CNAME, SNAME, PLOTFUNC, FPLOTFUNC)	\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0, SCM s_y0, SCM s_x1, SCM s_y1, SCM s_x2, SCM s_y2) \
  {									\
    plPlotter *c_plotter;						\
    int c_x0, c_y0, c_x1, c_y1, c_x2, c_y2;				\
    double c_dx0, c_dy0, c_dx1, c_dy1, c_dx2, c_dy2;			\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y0), s_y0, SCM_ARG3, SNAME);		\
    SCM_ASSERT (scm_is_real (s_x1), s_x1, SCM_ARG4, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y1), s_y1, SCM_ARG5, SNAME);		\
    SCM_ASSERT (scm_is_real (s_x2), s_x2, SCM_ARG6, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y2), s_y2, SCM_ARG7, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    if (_scm_is_exact_integer (s_x0) && _scm_is_exact_integer (s_y0)	\
	&& _scm_is_exact_integer (s_x1) && _scm_is_exact_integer (s_y1)	\
	&& _scm_is_exact_integer (s_x2) && _scm_is_exact_integer (s_y2)) \
      {									\
	c_x0 = scm_to_int (s_x0);					\
	c_y0 = scm_to_int (s_y0);					\
	c_x1 = scm_to_int (s_x1);					\
	c_y1 = scm_to_int (s_y1);					\
	c_x2 = scm_to_int (s_x2);					\
	c_y2 = scm_to_int (s_y2);					\
	return scm_from_int (PLOTFUNC (c_plotter, c_x0, c_y0, c_x1, c_y1, c_x2, c_y2)); \
      }									\
									\
    c_dx0 = scm_to_double (s_x0);					\
    c_dy0 = scm_to_double (s_y0);					\
    c_dx1 = scm_to_double (s_x1);					\
    c_dy1 = scm_to_double (s_y1);					\
    c_dx2 = scm_to_double (s_x2);					\
    c_dy2 = scm_to_double (s_y2);					\
    return scm_from_int (FPLOTFUNC (c_plotter, c_dx0, c_dy0, c_dx1, c_dy1, c_dx2, c_dy2)); \
  }

#define INT_FUNC_PL_DOUBLE_DOUBLE_DOUBLE_DOUBLE_DOUBLE_DOUBLE(CNAME, SNAME, FPLOTFUNC)	\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0, SCM s_y0, SCM s_x1, SCM s_y1, SCM s_x2, SCM s_y2) \
  {									\
    plPlotter *c_plotter;						\
    double c_dx0, c_dy0, c_dx1, c_dy1, c_dx2, c_dy2;			\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y0), s_y0, SCM_ARG3, SNAME);		\
    SCM_ASSERT (scm_is_real (s_x1), s_x1, SCM_ARG4, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y1), s_y1, SCM_ARG5, SNAME);		\
    SCM_ASSERT (scm_is_real (s_x2), s_x2, SCM_ARG6, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y2), s_y2, SCM_ARG7, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    c_dx0 = scm_to_double (s_x0);					\
    c_dy0 = scm_to_double (s_y0);					\
    c_dx1 = scm_to_double (s_x1);					\
    c_dy1 = scm_to_double (s_y1);					\
    c_dx2 = scm_to_double (s_x2);					\
    c_dy2 = scm_to_double (s_y2);					\
    return scm_from_int (FPLOTFUNC (c_plotter, c_dx0, c_dy0, c_dx1, c_dy1, c_dx2, c_dy2)); \
  }

#define INT_FUNC_PL_DOUBLE_DOUBLE(CNAME, SNAME, FPLOTFUNC)	\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0, SCM s_y0) \
  {									\
    plPlotter *c_plotter;						\
    double c_dx0, c_dy0;			\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y0), s_y0, SCM_ARG3, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    c_dx0 = scm_to_double (s_x0);					\
    c_dy0 = scm_to_double (s_y0);					\
    return scm_from_int (FPLOTFUNC (c_plotter, c_dx0, c_dy0)); \
  }




#define INT_FUNC_PL_STRING(CNAME, SNAME, FUNC)				\
  SCM									\
  CNAME (SCM s_plotter, SCM s_str)					\
  {									\
    plPlotter *c_plotter;						\
    char *c_str;							\
    int c_ret;								\
    									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_string (s_str), s_str, SCM_ARG2, SNAME);		\
    									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    c_str = scm_to_locale_string (s_str);				\
    c_ret = FUNC (c_plotter, c_str);					\
    free (c_str);							\
    return scm_from_int (c_ret);					\
  }



#define INT_FUNC_PL_INT(CNAME, SNAME, FUNC)				\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0)					\
  {									\
    plPlotter *c_plotter;						\
    int c_x0;								\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_integer (s_x0), s_x0, SCM_ARG2, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    c_x0 = scm_to_int (s_x0);						\
    return scm_from_int (FUNC (c_plotter, c_x0));			\
  }

#define INT_FUNC_PL_DOUBLE(CNAME, SNAME, FUNC)				\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0)					\
  {									\
    plPlotter *c_plotter;						\
    double c_x0;								\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    c_x0 = scm_to_double (s_x0);						\
    return scm_from_int (FUNC (c_plotter, c_x0));			\
  }


#define INT_FUNC_PL_VAL_VAL(CNAME, SNAME, PLOTFUNC, FPLOTFUNC)		\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0, SCM s_y0)				\
  {									\
    plPlotter *c_plotter;						\
    int c_x0, c_y0;							\
    double c_dx0, c_dy0;						\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y0), s_y0, SCM_ARG3, SNAME);		\
    									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    if (_scm_is_exact_integer (s_x0) && _scm_is_exact_integer (s_y0))	\
      {									\
	c_x0 = scm_to_int (s_x0);					\
	c_y0 = scm_to_int (s_y0);					\
	return scm_from_int (PLOTFUNC (c_plotter, c_x0, c_y0));		\
      }									\
									\
    c_dx0 = scm_to_double (s_x0);					\
    c_dy0 = scm_to_double (s_y0);					\
    return scm_from_int (FPLOTFUNC (c_plotter, c_dx0, c_dy0));		\
  }

#define INT_FUNC_PL_VAL_VAL_VAL(CNAME, SNAME, PLOTFUNC, FPLOTFUNC)	\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0, SCM s_y0, SCM s_v)			\
  {									\
    plPlotter *c_plotter;						\
    int c_x0, c_y0, c_v;						\
    double c_dx0, c_dy0, c_dv;						\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y0), s_y0, SCM_ARG3, SNAME);		\
    SCM_ASSERT (scm_is_real (s_v), s_v, SCM_ARG4, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    if (_scm_is_exact_integer (s_x0) && _scm_is_exact_integer (s_y0)	\
	&& _scm_is_exact_integer (s_v))					\
      {									\
	c_x0 = scm_to_int (s_x0);					\
	c_y0 = scm_to_int (s_y0);					\
	c_v = scm_to_int (s_v);						\
	return scm_from_int (PLOTFUNC (c_plotter, c_x0, c_y0, c_v));	\
      }									\
									\
    c_dx0 = scm_to_double (s_x0);					\
    c_dy0 = scm_to_double (s_y0);					\
    c_dv = scm_to_double (s_v);						\
    return scm_from_int (FPLOTFUNC (c_plotter, c_dx0, c_dy0, c_dv));	\
  }



#define INT_FUNC_PL_VAL(CNAME, SNAME, FUNC, FFUNC)			\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0)					\
  {									\
    plPlotter *c_plotter;						\
    int c_x0;								\
    double c_dx0;							\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    if (_scm_is_exact_integer (s_x0))					\
      {									\
	c_x0 = scm_to_int (s_x0);					\
	return scm_from_int (FUNC (c_plotter, c_x0));			\
      }									\
									\
    c_dx0 = scm_to_double (s_x0);					\
    return scm_from_int (FFUNC (c_plotter, c_dx0));			\
  }

#define INT_FUNC_PL_VAL(CNAME, SNAME, FUNC, FFUNC)			\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0)					\
  {									\
    plPlotter *c_plotter;						\
    int c_x0;								\
    double c_dx0;							\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    if (_scm_is_exact_integer (s_x0))					\
      {									\
	c_x0 = scm_to_int (s_x0);					\
	return scm_from_int (FUNC (c_plotter, c_x0));			\
      }									\
									\
    c_dx0 = scm_to_double (s_x0);					\
    return scm_from_int (FFUNC (c_plotter, c_dx0));			\
  }

#define VAL_FUNC_PL_VAL(CNAME, SNAME, FUNC, FFUNC)			\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0)					\
  {									\
    plPlotter *c_plotter;						\
    int c_x0;								\
    double c_dx0;							\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    if (_scm_is_exact_integer (s_x0))					\
      {									\
	c_x0 = scm_to_int (s_x0);					\
	return scm_from_int (FUNC (c_plotter, c_x0));			\
      }									\
									\
    c_dx0 = scm_to_double (s_x0);					\
    return scm_from_double (FFUNC (c_plotter, c_dx0));			\
  }


#define INT_FUNC_PL_VAL_VAL_VAL_VAL(CNAME, SNAME, PLOTFUNC, FPLOTFUNC)	\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0, SCM s_y0, SCM s_x1, SCM s_y1)		\
  {									\
    plPlotter *c_plotter;						\
    int c_x0, c_y0, c_x1, c_y1;						\
    double c_dx0, c_dy0, c_dx1, c_dy1;					\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y0), s_y0, SCM_ARG3, SNAME);		\
    SCM_ASSERT (scm_is_real (s_x1), s_x1, SCM_ARG4, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y1), s_y1, SCM_ARG5, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    if (_scm_is_exact_integer (s_x0) && _scm_is_exact_integer (s_y0)	\
	&& _scm_is_exact_integer (s_x1) && _scm_is_exact_integer (s_y1)) \
      {									\
	c_x0 = scm_to_int (s_x0);					\
	c_y0 = scm_to_int (s_y0);					\
	c_x1 = scm_to_int (s_x1);					\
	c_y1 = scm_to_int (s_y1);					\
	return scm_from_int (PLOTFUNC (c_plotter, c_x0, c_y0, c_x1, c_y1)); \
      }									\
									\
    c_dx0 = scm_to_double (s_x0);					\
    c_dy0 = scm_to_double (s_y0);					\
    c_dx1 = scm_to_double (s_x1);					\
    c_dy1 = scm_to_double (s_y1);					\
    return scm_from_int (FPLOTFUNC (c_plotter, c_dx0, c_dy0, c_dx1, c_dy1)); \
  }

#define INT_FUNC_PL_VAL_VAL_INT_VAL(CNAME, SNAME, PLOTFUNC, FPLOTFUNC)	\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0, SCM s_y0, SCM s_x1, SCM s_y1)		\
  {									\
    plPlotter *c_plotter;						\
    int c_x0, c_y0, c_x1, c_y1;						\
    double c_dx0, c_dy0, c_dy1;					\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y0), s_y0, SCM_ARG3, SNAME);		\
    SCM_ASSERT (scm_is_integer (s_x1), s_x1, SCM_ARG4, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y1), s_y1, SCM_ARG5, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    if (_scm_is_exact_integer (s_x0) && _scm_is_exact_integer (s_y0)	\
	&& _scm_is_exact_integer (s_x1) && _scm_is_exact_integer (s_y1)) \
      {									\
	c_x0 = scm_to_int (s_x0);					\
	c_y0 = scm_to_int (s_y0);					\
	c_x1 = scm_to_int (s_x1);					\
	c_y1 = scm_to_int (s_y1);					\
	return scm_from_int (PLOTFUNC (c_plotter, c_x0, c_y0, c_x1, c_y1)); \
      }									\
									\
    c_dx0 = scm_to_double (s_x0);					\
    c_dy0 = scm_to_double (s_y0);					\
    c_x1 = scm_to_int (s_x1);					\
    c_dy1 = scm_to_double (s_y1);					\
    return scm_from_int (FPLOTFUNC (c_plotter, c_dx0, c_dy0, c_x1, c_dy1)); \
  }


#define INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL(CNAME, SNAME, PLOTFUNC, FPLOTFUNC)	\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0, SCM s_y0, SCM s_x1, SCM s_y1, SCM s_v) \
  {									\
    plPlotter *c_plotter;						\
    int c_x0, c_y0, c_x1, c_y1, c_v;					\
    double c_dx0, c_dy0, c_dx1, c_dy1, c_dv;				\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y0), s_y0, SCM_ARG3, SNAME);		\
    SCM_ASSERT (scm_is_real (s_x1), s_x1, SCM_ARG4, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y1), s_y1, SCM_ARG5, SNAME);		\
    SCM_ASSERT (scm_is_real (s_v), s_v, SCM_ARG6, SNAME);		\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    if (_scm_is_exact_integer (s_x0) && _scm_is_exact_integer (s_y0)	\
	&& _scm_is_exact_integer (s_x1) && _scm_is_exact_integer (s_y1) \
	&& _scm_is_exact_integer (s_v))					\
      {									\
	c_x0 = scm_to_int (s_x0);					\
	c_y0 = scm_to_int (s_y0);					\
	c_x1 = scm_to_int (s_x1);					\
	c_y1 = scm_to_int (s_y1);					\
	c_v = scm_to_int (s_v);						\
	return scm_from_int (PLOTFUNC (c_plotter, c_x0, c_y0, c_x1, c_y1, c_v)); \
      }									\
									\
    c_dx0 = scm_to_double (s_x0);					\
    c_dy0 = scm_to_double (s_y0);					\
    c_dx1 = scm_to_double (s_x1);					\
    c_dy1 = scm_to_double (s_y1);					\
    c_dv = scm_to_double (s_v);						\
    return scm_from_int (FPLOTFUNC (c_plotter, c_dx0, c_dy0, c_dx1, c_dy1, c_dv)); \
  }



#define INT_FUNC_PL_VAL_VAL_VAL_VAL_VAL_VAL_VAL_VAL(CNAME, SNAME, PLOTFUNC, FPLOTFUNC)		\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0, SCM s_y0, SCM s_x1, SCM s_y1, SCM s_x2, SCM s_y2, SCM s_x3, SCM s_y3) \
  {									\
    plPlotter *c_plotter;						\
    int c_x0, c_y0, c_x1, c_y1, c_x2, c_y2, c_x3, c_y3;			\
    double c_dx0, c_dy0, c_dx1, c_dy1, c_dx2, c_dy2, c_dx3, c_dy3;	\
									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_real (s_x0), s_x0, SCM_ARG2, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y0), s_y0, SCM_ARG3, SNAME);		\
    SCM_ASSERT (scm_is_real (s_x1), s_x1, SCM_ARG4, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y1), s_y1, SCM_ARG5, SNAME);		\
    SCM_ASSERT (scm_is_real (s_x2), s_x2, SCM_ARG6, SNAME);		\
    SCM_ASSERT (scm_is_real (s_y2), s_y2, SCM_ARG7, SNAME);		\
    SCM_ASSERT (scm_is_real (s_x3), s_x3, 8, SNAME);			\
    SCM_ASSERT (scm_is_real (s_y3), s_y3, 9, SNAME);			\
									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    if (_scm_is_exact_integer (s_x0) && _scm_is_exact_integer (s_y0)	\
	&& _scm_is_exact_integer (s_x1) && _scm_is_exact_integer (s_y1)	\
	&& _scm_is_exact_integer (s_x2) && _scm_is_exact_integer (s_y2)	\
	&& _scm_is_exact_integer (s_x3) && _scm_is_exact_integer (s_y3)) \
      {									\
	c_x0 = scm_to_int (s_x0);					\
	c_y0 = scm_to_int (s_y0);					\
	c_x1 = scm_to_int (s_x1);					\
	c_y1 = scm_to_int (s_y1);					\
	c_x2 = scm_to_int (s_x2);					\
	c_y2 = scm_to_int (s_y2);					\
	c_x3 = scm_to_int (s_x3);					\
	c_y3 = scm_to_int (s_y3);					\
	return scm_from_int (PLOTFUNC (c_plotter, c_x0, c_y0, c_x1, c_y1, c_x2, c_y2, c_x3, c_y3)); \
      }									\
									\
    c_dx0 = scm_to_double (s_x0);					\
    c_dy0 = scm_to_double (s_y0);					\
    c_dx1 = scm_to_double (s_x1);					\
    c_dy1 = scm_to_double (s_y1);					\
    c_dx2 = scm_to_double (s_x2);					\
    c_dy2 = scm_to_double (s_y2);					\
    c_dx3 = scm_to_double (s_x3);					\
    c_dy3 = scm_to_double (s_y3);					\
    return scm_from_int (FPLOTFUNC (c_plotter, c_dx0, c_dy0, c_dx1, c_dy1, c_dx2, c_dy2, c_dx3, c_dy3)); \
  }

#define DOUBLE_FUNC_ONE_DOUBLE(CNAME, SNAME, FUNC)			\
  SCM									\
  CNAME (SCM s_plotter, SCM s_x0)					\
  {									\
    plPlotter *c_plotter;						\
    double c_x0;							\
    									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_read (s_x0), s_x0, SCM_ARG2, SNAME);		\
    									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    c_x0 = scm_to_double (s_x0);					\
    return scm_from_double (FUNC (c_plotter, c_x0));			\
  }

#define DOUBLE_FUNC_PL_STRING(CNAME, SNAME, FUNC)			\
  SCM									\
  CNAME (SCM s_plotter, SCM s_str)					\
  {									\
    plPlotter *c_plotter;						\
    char *c_str;							\
    double c_ret;							\
    									\
    SCM_ASSERT (_scm_is_plotter (s_plotter), s_plotter, SCM_ARG1, SNAME); \
    SCM_ASSERT (scm_is_string (s_str), s_str, SCM_ARG2, SNAME);		\
    									\
    c_plotter = _scm_to_plotter (s_plotter);				\
    c_str = scm_to_locale_string (s_str);				\
    c_ret = FUNC (c_plotter, c_str);					\
    free (c_str);							\
    return scm_from_double (c_ret);					\
  }



#endif
