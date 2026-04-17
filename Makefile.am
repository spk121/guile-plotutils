# Makefile.am - top level
# Copyright (C) 2018, 2019, 2020, 2021, 2026 Michael L. Gran

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
AUTOMAKE_OPTIONS = subdir-objects
AM_DISTCHECK_CONFIGURE_FLAGS = --with-gnu-filesystem-hierarchy
AM_DISTCHECK_DVI_TARGET = html

#################
#  Directories  #
#################

guilesitedir = $(GUILE_SITE)
guileextensiondir = $(GUILE_EXTENSION)
guileobjectdir = $(GUILE_SITE_CCACHE)
pkgguilesitedir = $(guilesitedir)/plotutils
pkgguileobjectdir = $(guileobjectdir)/plotutils

#################
#   Top Level   #
#################

dist: \
  README

README: README.md
	$(LN_S) $< $@

########################
#  libguile-plotutils  #
########################

guileextension_LTLIBRARIES = libguile-plotutils.la

libguile_plotutils_la_c_sources = \
  src/compat.c \
  src/display.c \
  src/fontlist.c \
  src/guile_graph.c \
  src/guile_plot.c \
  src/linemode.c \
  src/misc.c \
  src/plotter.c \
  src/reader.c \
  src/xmalloc.c \
  src/xstrdup.c

libguile_plotutils_la_internal_headers = \
  src/guile_graph.h \
  src/guile_plot.h \
  src/plot.h \
  src/visibility.h \
  src/libcommon.h \
  src/sys-defines.h \
  src/fontlist.h \
  src/extern.h

libguile_plotutils_la_SOURCES = \
  $(libguile_plotutils_la_internal_headers) \
  $(libguile_plotutils_la_c_sources)

libguile_plotutils_la_CPPFLAGS = \
  -DHAVE_STRERROR=1 \
  -UX_DISPLAY_MISSING \
  -DINCLUDE_PNG_SUPPORT=1

if BUILDING_DLL
libguile_plotutils_la_CPPFLAGS += -DGUPL_DLL -DGUPL_DLL_EXPORTS
endif

libguile_plotutils_la_CFLAGS = \
  -std=c11 \
  $(GUILE_CFLAGS)

libguile_plotutils_la_LDFLAGS = \
 -no-undefined \
 -version-info $(LIBGUILE_PLOTUTILS_INTERFACE) \
 -export-symbols-regex '^gupl_' \
 $(GUILE_LDFLAGS)

if !DLL_VERSION_INFO
libguile_plotutils_la_LDFLAGS += -avoid-version
endif

libguile_plotutils_la_LIBADD = \
 $(GUILE_LIBS) \
 -lplot

#################
# Guile Modules #
#################

dist_pkgguilesite_DATA = \
  module/plotutils/plot.scm \
  module/plotutils/graph.scm

if GUILD
guileobject_DATA = \
  $(dist_guilesite_DATA:%.scm=%.go)

pkgguileobject_DATA = \
  $(dist_pkgguilesite_DATA:%.scm=%.go) \
  $(nodist_pkgguilesite_DATA:%.scm=%.go)

GUILEC_FLAGS = \
 -O2 \
 -Warity-mismatch \
 -Wformat \
 -Wmacro-use-before-definition \
 -Wunbound-variable \
 --load-path=$(abs_srcdir)/module \
 --load-path=$(abs_builddir)/module

.scm.go:
	GUILE_AUTO_COMPILE=0 \
	LTDL_LIBRARY_PATH=$(abs_builddir)/.libs \
	$(GUILE_TOOLS) compile --target="$(host)" $(GUILEC_FLAGS) \
	-o "$@" "$<"

$(guileobject_DATA) $(pkgguileobject_DATA): $(guileextension_LTLIBRARIES)
endif

#################
#     Tests     #
#################

TEST_EXTENSIONS = .scm

EVERYTHING_TESTS =
TESTS_LTDLOPEN = -dlopen ${top_builddir}/libguile-plotutils.la

TESTS = \
  test/_setup.scm \
  test/plot.scm \
  test/graph.scm

XFAIL_TESTS =

SCM_LOG_DRIVER = \
  $(top_builddir)/tools/uninstalled-test-env \
  $(top_builddir)/libtool $(TESTS_LTDLOPEN) --mode=execute \
  $(GUILE) --no-auto-compile $(top_srcdir)/build-aux/test-driver.scm

test: check

#################
#  Local rules  #
#################

# Prerequisites needed by docs/Makefile.am before generating images.
docs-prereqs: libguile-plotutils.la $(guileobject_DATA) $(pkgguileobject_DATA)

check-syntax:
	$(CC) -std=c11 $(GUILE_CFLAGS) $(GLIB_CFLAGS) $(GOBJECT_CFLAGS) \
	 $(GOBJECT_INTROSPECTION_CFLAGS) $(FFI_CFLAGS) \
	 -DG_LOG_DOMAIN=\"GuileGI\" $(CFLAGS) -fsyntax-only $(libguile_plotutils_la_c_sources)

indent:
	VERSION_CONTROL=none indent $(libguile_plotutils_la_SOURCES)

.PHONY: check-syntax docs-prereqs indent test

#################
#   Variables   #
#################

CLEANFILES = \
  ChangeLog README \
  $(guileobject_DATA) $(pkgguileobject_DATA) \
  $(check_DATA) \
  *.log



EXTRA_DIST = \
  README.md \
  build-aux/config.rpath \
  m4/attributes.m4 \
  tools/uninstalled-env.in \
  tools/uninstalled-test-env.in \
  tools/run-guile.in \
  tools/run-test.in \
  tools/gdb-guile.in \
  tools/gdb-test.in \
  $(TESTS) \
  $(noinst_DATA) \
  docs

SUBDIRS = . docs
