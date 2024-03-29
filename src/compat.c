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

/* Replacement for system strerror() if it doesn't exist. */

#ifndef HAVE_STRERROR

/* external */
extern char *sys_errlist[];
extern int sys_nerr;

char *
strerror (int errnum)
{
  if (errnum < 0 || errnum >= sys_nerr)
    return "unknown error";

  return sys_errlist[errnum];
}
#endif /* not HAVE_STRERROR */
