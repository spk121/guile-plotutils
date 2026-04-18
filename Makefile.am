## Makefile.am -- top level
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

# Build src/ first (produces libguile-plotutils.la), then the current
# directory (compiles the Guile modules, which depend on the library),
# then docs/ (which in maintainer mode runs scripts that load the
# library).
SUBDIRS = src module/plotutils . docs

#################
#  Directories  #
#################

guilesitedir = $(GUILE_SITE)
guileobjectdir = $(GUILE_SITE_CCACHE)
pkgguilesitedir = $(guilesitedir)/plotutils
pkgguileobjectdir = $(guileobjectdir)/plotutils

#################
#   Top Level   #
#################

dist: README

README: README.md
	$(LN_S) $< $@

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
	LTDL_LIBRARY_PATH=$(abs_builddir)/src/.libs \
	$(GUILE_TOOLS) compile --target="$(host)" $(GUILEC_FLAGS) \
	  -o "$@" "$<"

$(guileobject_DATA) $(pkgguileobject_DATA): src/libguile-plotutils.la
endif

#################
#     Tests     #
#################

TEST_EXTENSIONS = .scm

TESTS_LTDLOPEN = -dlopen $(top_builddir)/src/libguile-plotutils.la

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

# Prerequisites needed by docs/Makefile.am before regenerating images
# in maintainer mode.
docs-prereqs: src/libguile-plotutils.la $(guileobject_DATA) $(pkgguileobject_DATA)

.PHONY: docs-prereqs test

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
  NEWS \
  build-aux/config.rpath \
  m4/attributes.m4 \
  tools/uninstalled-env.in \
  tools/uninstalled-test-env.in \
  tools/run-guile.in \
  tools/run-test.in \
  tools/gdb-guile.in \
  tools/gdb-test.in \
  $(TESTS) \
  $(noinst_DATA)
