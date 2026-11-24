/* This file is part of the GNU plotutils package.  Copyright (C) 1995,
   1996, 1997, 1998, 1999, 2000, 2005, 2008, Free Software Foundation, Inc.

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

/* fontlist.h: header file for fontlist.c, containing functions that access
   the internal database of fonts in libplot/libplotter, via an
   undocumented interface. */

/* Support C++.  This file could be #included by a C++ compiler rather than
   a C compiler, in which case it needs to know that fontlist.c functions
   have C linkage, not C++ linkage.  This is accomplished by wrapping all
   function declarations in __BEGIN_DECLS ... __END_DECLS. */
#ifdef ___BEGIN_DECLS
#undef ___BEGIN_DECLS
#endif
#ifdef ___END_DECLS
#undef ___END_DECLS
#endif
#ifdef __cplusplus
#define ___BEGIN_DECLS                                                        \
  extern "C"                                                                  \
  {
#define ___END_DECLS }
#else
#define ___BEGIN_DECLS /* empty */
#define ___END_DECLS   /* empty */
#endif

#include "plot.h" /* plPlotter */

___BEGIN_DECLS

/* Call CALLBACK with the name of each font that PLOTTER supports, and
   DATA, in the order GNU graph --list-fonts uses.  Returns the number of
   fonts, or -1 if libplot's font tables have a layout this code doesn't
   recognize.  (Most X Plotters can also use many core X fonts not
   listed.) */
extern int foreach_plotter_font (plPlotter *plotter,
                                 void (*callback) (const char *name,
                                                   void *data),
                                 void *data);

___END_DECLS
