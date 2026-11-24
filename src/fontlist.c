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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "plot.h"

#include "fontlist.h"

/* The font structures below are copied from libplot's libplot/extern.h,
   which is not installed.  IF THOSE STRUCTURES CHANGE, THESE SHOULD TOO.

   Their layout depends on how libplot was built.  libplot's `bool' is
   defined in its include/sys-defines.h: plotutils 2.6 as released made it
   an enum (the size of an int), but distributions building with GCC 15,
   such as Debian and Ubuntu from plotutils 2.6-15, patch it to C99's bool
   (usually one byte), which shrinks the structures.  So each is defined
   twice, for either size of bool, and the one to use is found at run
   time (see find_layout below).  */

typedef enum
{
  pl_enum_false = 0,
  pl_enum_true = 1
} pl_enum_bool;

#define DEFINE_FONT_STRUCTS(SUFFIX, BOOL)                                     \
  struct hershey_##SUFFIX                                                     \
  {                                                                           \
    const char *name;      /* font name */                                    \
    const char *othername; /* an alias (for backward compatibility) */        \
    const char *orig_name; /* Allen Hershey's original name for it */         \
    short chars[256];      /* array of vector glyphs */                       \
    int typeface_index;    /* default typeface for the font */                \
    int font_index;        /* which font within typeface this is */           \
    BOOL obliquing;        /* whether to apply obliquing */                   \
    BOOL iso8859_1;        /* whether font encoding is iso8859-1 */           \
    BOOL visible;          /* whether font is visible, i.e. not internal */   \
  };                                                                          \
                                                                              \
  struct ps_##SUFFIX                                                          \
  {                                                                           \
    const char *ps_name;      /* the postscript font name */                  \
    const char *ps_name_alt;  /* alternative PS font name, if non-NULL */     \
    const char *ps_name_alt2; /* 2nd alternative PS font name, if non-NULL */ \
    const char *x_name;       /* the X Windows font name */                   \
    const char *x_name_alt;   /* alternative X Windows font name */           \
    const char *x_name_alt2;  /* 2nd alternative X Windows font name */       \
    const char *x_name_alt3;  /* 3rd alternative X Windows font name */       \
    const char *css_family;   /* CSS font family */                           \
    const char *css_generic_family; /* CSS generic font family */             \
    const char *css_style;          /* CSS font style */                      \
    const char *css_weight;         /* CSS font weight */                     \
    const char *css_stretch;        /* CSS font stretch */                    \
    const char *css_panose;         /* CSS font Panose */                     \
    int pcl_typeface;               /* the PCL typeface number */             \
    int hpgl_spacing;               /* 0=fixed width, 1=variable */           \
    int hpgl_posture;               /* 0=upright, 1=italic, etc. */           \
    int hpgl_stroke_weight;         /* 0=normal, 3=bold, 4=extra bold */      \
    int hpgl_symbol_set;            /* 0=Roman-8, 14=ISO-8859-1, etc. */      \
    int font_ascent;                /* the font's ascent */                   \
    int font_descent;               /* the font's descent */                  \
    int font_cap_height;            /* the font's cap height */               \
    int font_x_height;              /* the font's x height */                 \
    short width[256];               /* per-character width information */     \
    short offset[256];              /* per-character left edge information */ \
    int typeface_index;             /* default typeface for the font */       \
    int font_index;                 /* which font within typeface this is */  \
    int fig_id;                     /* Fig's font id */                       \
    BOOL iso8859_1;                 /* whether font encoding is iso8859-1 */  \
  };                                                                          \
                                                                              \
  struct pcl_##SUFFIX                                                         \
  {                                                                           \
    const char *ps_name;            /* the postscript font name */            \
    const char *ps_name_alt;        /* alternative PS font name, if any */    \
    const char *substitute_ps_name; /* replacement name (in a PS file) */     \
    const char *x_name;             /* the X Windows font name */             \
    const char *css_family;         /* CSS font family */                     \
    const char *css_generic_family; /* CSS generic font family */             \
    const char *css_style;          /* CSS font style */                      \
    const char *css_weight;         /* CSS font weight */                     \
    const char *css_stretch;        /* CSS font stretch */                    \
    const char *css_panose;         /* CSS font Panose */                     \
    int pcl_typeface;               /* the PCL typeface number */             \
    int hpgl_spacing;               /* 0=fixed width, 1=variable */           \
    int hpgl_posture;               /* 0=upright, 1=italic, etc. */           \
    int hpgl_stroke_weight;         /* 0=normal, 3=bold, 4=extra bold */      \
    int hpgl_symbol_set;            /* 0=Roman-8, 14=ISO-8859-1, etc. */      \
    int font_ascent;                /* the font's ascent */                   \
    int font_descent;               /* the font's descent */                  \
    int font_cap_height;            /* the font's cap height */               \
    int font_x_height;              /* the font's x height */                 \
    short width[256];               /* per-character width information */     \
    short offset[256];              /* per-character left edge information */ \
    int typeface_index;             /* default typeface for the font */       \
    int font_index;                 /* which font within typeface this is */  \
    BOOL iso8859_1;                 /* whether font encoding is iso8859-1 */  \
  };                                                                          \
                                                                              \
  struct stick_##SUFFIX                                                       \
  {                                                                           \
    const char *ps_name;     /* the postscript font name */                   \
    BOOL basic;              /* basic stick font (supp. on all devices)? */   \
    int pcl_typeface;        /* the PCL typeface number */                    \
    int hpgl_spacing;        /* 0=fixed width, 1=variable */                  \
    int hpgl_posture;        /* 0=upright, 1=italic, etc. */                  \
    int hpgl_stroke_weight;  /* 0=normal, 3=bold, 4=extra bold, etc. */       \
    int hpgl_symbol_set;     /* 0=Roman-8, 14=ISO-8859-1 */                   \
    int font_ascent;         /* the font's ascent (from bounding box) */      \
    int font_descent;        /* the font's descent (from bounding box) */     \
    int raster_width_lower;  /* width of abstract raster (lower half) */      \
    int raster_height_lower; /* height of abstract raster (lower half) */     \
    int raster_width_upper;  /* width of abstract raster (upper half) */      \
    int raster_height_upper; /* height of abstract raster (upper half) */     \
    int hpgl_charset_lower;  /* old HP character set number (lower half) */   \
    int hpgl_charset_upper;  /* old HP character set number (upper half) */   \
    int kerning_table_lower; /* number of a kerning table (lower half) */     \
    int kerning_table_upper; /* number of a kerning table (upper half) */     \
    char width[256];         /* per-character width information */           \
    int offset;              /* left edge (applies to all chars) */           \
    int typeface_index;      /* default typeface for the font */              \
    int font_index;          /* which font within typeface this is */         \
    BOOL obliquing;          /* whether to apply obliquing */                 \
    BOOL iso8859_1;          /* encoding is iso8859-1? (after reencoding) */  \
  };

DEFINE_FONT_STRUCTS (enum_bool, pl_enum_bool) /* plotutils 2.6 as released */
DEFINE_FONT_STRUCTS (c99_bool, bool)          /* patched for GCC 15 */

/* Report each font in PLOTTER's font tables, read with the structures
   for SUFFIX, to CALLBACK; return the number of fonts.  */
#define DEFINE_FOREACH_FONT(SUFFIX)                                           \
  static int foreach_font_##SUFFIX (                                          \
      plPlotter *plotter, void (*callback) (const char *name, void *data),    \
      void *data)                                                             \
  {                                                                           \
    int count = 0;                                                            \
    int i;                                                                    \
                                                                              \
    /* Hershey vector fonts (some are internal, i.e. not visible) */          \
    if (pl_havecap_r (plotter, "HERSHEY_FONTS"))                              \
      {                                                                       \
        const struct hershey_##SUFFIX *info                                   \
            = (const struct hershey_##SUFFIX *)_pl_get_hershey_font_info (    \
                plotter);                                                     \
        for (i = 0; info[i].name; i++)                                        \
          if (info[i].visible)                                                \
            {                                                                 \
              callback (info[i].name, data);                                  \
              count++;                                                        \
            }                                                                 \
      }                                                                       \
                                                                              \
    /* HP vector (stick) fonts; the extra ones only where supported */        \
    if (pl_havecap_r (plotter, "STICK_FONTS"))                                \
      {                                                                       \
        const struct stick_##SUFFIX *info                                     \
            = (const struct stick_##SUFFIX *)_pl_get_stick_font_info (        \
                plotter);                                                     \
        int extra_fonts = pl_havecap_r (plotter, "EXTRA_STICK_FONTS");        \
        for (i = 0; info[i].ps_name; i++)                                     \
          if (extra_fonts || info[i].basic)                                   \
            {                                                                 \
              callback (info[i].ps_name, data);                               \
              count++;                                                        \
            }                                                                 \
      }                                                                       \
                                                                              \
    /* PCL fonts */                                                           \
    if (pl_havecap_r (plotter, "PCL_FONTS"))                                  \
      {                                                                       \
        const struct pcl_##SUFFIX *info                                       \
            = (const struct pcl_##SUFFIX *)_pl_get_pcl_font_info (plotter);   \
        for (i = 0; info[i].ps_name; i++)                                     \
          {                                                                   \
            callback (info[i].ps_name, data);                                 \
            count++;                                                          \
          }                                                                   \
      }                                                                       \
                                                                              \
    /* Postscript fonts */                                                    \
    if (pl_havecap_r (plotter, "PS_FONTS"))                                   \
      {                                                                       \
        const struct ps_##SUFFIX *info                                        \
            = (const struct ps_##SUFFIX *)_pl_get_ps_font_info (plotter);     \
        for (i = 0; info[i].ps_name; i++)                                     \
          {                                                                   \
            callback (info[i].ps_name, data);                                 \
            count++;                                                          \
          }                                                                   \
      }                                                                       \
                                                                              \
    return count;                                                             \
  }

DEFINE_FOREACH_FONT (enum_bool)
DEFINE_FOREACH_FONT (c99_bool)

typedef enum
{
  LAYOUT_UNKNOWN,
  LAYOUT_ENUM_BOOL,
  LAYOUT_C99_BOOL
} font_layout;

/* Whether POINTER could be a font name in libplot's tables: not NULL,
   and close to NAME0, the first font's name (they are all string
   constants in libplot), without dereferencing it.  */
static bool
near_name (uintptr_t pointer, const char *name0)
{
  uintptr_t n0 = (uintptr_t)name0;
  uintptr_t distance = (pointer > n0 ? pointer - n0 : n0 - pointer);

  return pointer != 0 && distance < ((uintptr_t)1 << 24);
}

/* Whether the second Hershey font, read with structures of SIZE bytes,
   has a plausible name.  */
static bool
second_hershey_name_ok (const char *table, size_t size, const char *name0)
{
  const char *name;

  memcpy (&name, table + size, sizeof name); /* entry 1's first field */
  return near_name ((uintptr_t)name, name0)
         && strncmp (name, "Hershey", 7) == 0;
}

/* Find which layout libplot's font structures have, from its Hershey
   font table, whose first two entries are always visible Hershey fonts.
   Every entry starts with its name, so entry 0's name is at the start of
   the table whatever the layout.  The structures differ only in their
   trailing bool fields, so the smaller (C99 bool) structure ends inside
   the larger one's trailing int-sized fields: where entry 1 would start
   in the C99 layout, the enum layout has small integers, not a pointer
   near entry 0's name.  That decides without dereferencing anything
   uncertain; the second entry's name is then checked before use.  */
static font_layout
find_layout (plPlotter *plotter)
{
  const char *table = (const char *)_pl_get_hershey_font_info (plotter);
  const char *name0;
  uintptr_t candidate;

  if (table == NULL)
    return LAYOUT_UNKNOWN;
  memcpy (&name0, table, sizeof name0);
  if (name0 == NULL || strncmp (name0, "Hershey", 7) != 0)
    return LAYOUT_UNKNOWN;

  /* If bool is the size of an int, the layouts are the same. */
  if (sizeof (struct hershey_c99_bool) == sizeof (struct hershey_enum_bool))
    return (second_hershey_name_ok (table, sizeof (struct hershey_enum_bool),
                                    name0)
                ? LAYOUT_ENUM_BOOL
                : LAYOUT_UNKNOWN);

  memcpy (&candidate, table + sizeof (struct hershey_c99_bool),
          sizeof candidate);
  if (near_name (candidate, name0))
    return (second_hershey_name_ok (table, sizeof (struct hershey_c99_bool),
                                    name0)
                ? LAYOUT_C99_BOOL
                : LAYOUT_UNKNOWN);
  return (second_hershey_name_ok (table, sizeof (struct hershey_enum_bool),
                                  name0)
              ? LAYOUT_ENUM_BOOL
              : LAYOUT_UNKNOWN);
}

int
foreach_plotter_font (plPlotter *plotter,
                      void (*callback) (const char *name, void *data),
                      void *data)
{
  /* The layout is the loaded libplot's, the same for every Plotter. */
  static font_layout layout = LAYOUT_UNKNOWN;

  if (layout == LAYOUT_UNKNOWN)
    layout = find_layout (plotter);
  switch (layout)
    {
    case LAYOUT_ENUM_BOOL:
      return foreach_font_enum_bool (plotter, callback, data);
    case LAYOUT_C99_BOOL:
      return foreach_font_c99_bool (plotter, callback, data);
    default:
      return -1;
    }
}
