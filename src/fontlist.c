/* This file is part of the GNU plotutils package.  Copyright (C) 1995,
   1996, 1997, 1998, 1999, 2000, 2005, 2008, 2009, Free Software
   Foundation, Inc.

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

/* This file contains foreach_plotter_font(), which reports the names of
   the fonts a libplot Plotter supports.  (It replaces the display_fonts
   and list_fonts routines of GNU plotutils, which printed them.)  It is
   declared in fontlist.h.

   libplot has no public API for this, so it uses undocumented members of
   the libplot API, which return pointers to internal library data
   structures: _pl_get_hershey_font_info, _pl_get_ps_font_info,
   _pl_get_pcl_font_info, and _pl_get_stick_font_info. */

#include "plot.h"

#include "fontlist.h"

/* libplot's own `bool' type (from its sys-defines.h), which is an enum,
   i.e. the size of an int, not the one-byte C99 bool.  The structures
   below must use it, or they won't match libplot's layout and indexing
   its font arrays reads garbage.  */
typedef enum
{
  pl_false = 0,
  pl_true = 1
} pl_bool;

/* The definitions of these structures are taken from ../libplot/extern.h.
   IF THOSE STRUCTURES CHANGE, THESE SHOULD TOO.

   Font information is stored in ../libplot/g_fontdb.c, and we'll retrieve
   pointers to it by using the undocumented libplot functions. */

struct plHersheyFontInfoStruct
{
  const char *name;      /* font name */
  const char *othername; /* an alias (for backward compatibility) */
  const char *orig_name; /* Allen Hershey's original name for it */
  short chars[256];      /* array of vector glyphs */
  int typeface_index;    /* default typeface for the font */
  int font_index;        /* which font within typeface this is */
  pl_bool obliquing;        /* whether to apply obliquing */
  pl_bool iso8859_1;        /* whether font encoding is iso8859-1 */
  pl_bool visible;          /* whether font is visible, i.e. not internal */
};

struct plPSFontInfoStruct
{
  const char *ps_name;      /* the postscript font name */
  const char *ps_name_alt;  /* alternative PS font name, if non-NULL */
  const char *ps_name_alt2; /* 2nd alternative PS font name, if non-NULL */
  const char *x_name;       /* the X Windows font name */
  const char *x_name_alt;   /* alternative X Windows font name */
  const char *x_name_alt2;  /* 2nd alternative X Windows font name */
  const char *x_name_alt3;  /* 3rd alternative X Windows font name */
  const char *css_family;   /* CSS font family */
  const char *css_generic_family; /* CSS generic font family */
  const char *css_style;          /* CSS font style */
  const char *css_weight;         /* CSS font weight */
  const char *css_stretch;        /* CSS font stretch */
  const char *css_panose;         /* CSS font Panose */
  int pcl_typeface;               /* the PCL typeface number */
  int hpgl_spacing;               /* 0=fixed width, 1=variable */
  int hpgl_posture;               /* 0=upright, 1=italic, etc. */
  int hpgl_stroke_weight;         /* 0=normal, 3=bold, 4=extra bold, etc. */
  int hpgl_symbol_set;            /* 0=Roman-8, 14=ISO-8859-1, etc. */
  int font_ascent;                /* the font's ascent (from bounding box) */
  int font_descent;               /* the font's descent (from bounding box) */
  int font_cap_height;            /* the font's cap height */
  int font_x_height;              /* the font's x height */
  short width[256];               /* per-character width information */
  short offset[256];              /* per-character left edge information */
  int typeface_index;             /* default typeface for the font */
  int font_index;                 /* which font within typeface this is */
  int fig_id;                     /* Fig's font id */
  pl_bool iso8859_1;                 /* whether font encoding is iso8859-1 */
};

struct plPCLFontInfoStruct
{
  const char *ps_name;            /* the postscript font name */
  const char *ps_name_alt;        /* alternative PS font name, if non-NULL */
  const char *substitute_ps_name; /* replacement name (for use in a PS file) */
  const char *x_name;             /* the X Windows font name */
  const char *css_family;         /* CSS font family */
  const char *css_generic_family; /* CSS generic font family */
  const char *css_style;          /* CSS font style */
  const char *css_weight;         /* CSS font weight */
  const char *css_stretch;        /* CSS font stretch */
  const char *css_panose;         /* CSS font Panose */
  int pcl_typeface;               /* the PCL typeface number */
  int hpgl_spacing;               /* 0=fixed width, 1=variable */
  int hpgl_posture;               /* 0=upright, 1=italic, etc. */
  int hpgl_stroke_weight;         /* 0=normal, 3=bold, 4=extra bold, etc. */
  int hpgl_symbol_set;            /* 0=Roman-8, 14=ISO-8859-1, etc. */
  int font_ascent;                /* the font's ascent (from bounding box) */
  int font_descent;               /* the font's descent (from bounding box) */
  int font_cap_height;            /* the font's cap height */
  int font_x_height;              /* the font's x height */
  short width[256];               /* per-character width information */
  short offset[256];              /* per-character left edge information */
  int typeface_index;             /* default typeface for the font */
  int font_index;                 /* which font within typeface this is */
  pl_bool iso8859_1;                 /* whether font encoding is iso8859-1 */
};

struct plStickFontInfoStruct
{
  const char *ps_name;     /* the postscript font name */
  pl_bool basic;              /* basic stick font (supp. on all devices)? */
  int pcl_typeface;        /* the PCL typeface number */
  int hpgl_spacing;        /* 0=fixed width, 1=variable */
  int hpgl_posture;        /* 0=upright, 1=italic, etc. */
  int hpgl_stroke_weight;  /* 0=normal, 3=bold, 4=extra bold, etc. */
  int hpgl_symbol_set;     /* 0=Roman-8, 14=ISO-8859-1 */
  int font_ascent;         /* the font's ascent (from bounding box) */
  int font_descent;        /* the font's descent (from bounding box) */
  int raster_width_lower;  /* width of abstract raster (lower half) */
  int raster_height_lower; /* height of abstract raster (lower half) */
  int raster_width_upper;  /* width of abstract raster (upper half) */
  int raster_height_upper; /* height of abstract raster (upper half) */
  int hpgl_charset_lower;  /* old HP character set number (lower half) */
  int hpgl_charset_upper;  /* old HP character set number (upper half) */
  int kerning_table_lower; /* number of a kerning table (lower half) */
  int kerning_table_upper; /* number of a kerning table (upper half) */
  char width[256];         /* per-character width information */
  int offset;              /* left edge (applies to all chars) */
  int typeface_index;      /* default typeface for the font */
  int font_index;          /* which font within typeface this is */
  pl_bool obliquing;          /* whether to apply obliquing */
  pl_bool iso8859_1;          /* encoding is iso8859-1? (after reencoding) */
};

int
foreach_plotter_font (plPlotter *plotter,
                      void (*callback) (const char *name, void *data),
                      void *data)
{
  int count = 0;
  int i;

  /* Hershey vector fonts (some are internal, i.e. not visible) */
  if (pl_havecap_r (plotter, "HERSHEY_FONTS"))
    {
      const struct plHersheyFontInfoStruct *hershey_font_info
          = (const struct plHersheyFontInfoStruct *)_pl_get_hershey_font_info (
              plotter);

      for (i = 0; hershey_font_info[i].name; i++)
        if (hershey_font_info[i].visible)
          {
            callback (hershey_font_info[i].name, data);
            count++;
          }
    }

  /* HP vector (stick) fonts; the extra ones only where supported */
  if (pl_havecap_r (plotter, "STICK_FONTS"))
    {
      const struct plStickFontInfoStruct *stick_font_info
          = (const struct plStickFontInfoStruct *)_pl_get_stick_font_info (
              plotter);
      int extra_fonts = pl_havecap_r (plotter, "EXTRA_STICK_FONTS");

      for (i = 0; stick_font_info[i].ps_name; i++)
        {
          if (!extra_fonts && !stick_font_info[i].basic)
            continue;
          callback (stick_font_info[i].ps_name, data);
          count++;
        }
    }

  /* PCL fonts */
  if (pl_havecap_r (plotter, "PCL_FONTS"))
    {
      const struct plPCLFontInfoStruct *pcl_font_info
          = (const struct plPCLFontInfoStruct *)_pl_get_pcl_font_info (
              plotter);

      for (i = 0; pcl_font_info[i].ps_name; i++)
        {
          callback (pcl_font_info[i].ps_name, data);
          count++;
        }
    }

  /* Postscript fonts */
  if (pl_havecap_r (plotter, "PS_FONTS"))
    {
      const struct plPSFontInfoStruct *ps_font_info
          = (const struct plPSFontInfoStruct *)_pl_get_ps_font_info (plotter);

      for (i = 0; ps_font_info[i].ps_name; i++)
        {
          callback (ps_font_info[i].ps_name, data);
          count++;
        }
    }

  return count;
}
