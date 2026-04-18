## module/plotutils/Makefile.am -- Guile modules

# ------------------------------------------------------------------
# Install locations
# ------------------------------------------------------------------
# The .scm sources install under $(GUILE_SITE)/plotutils, and the
# compiled .go objects under $(GUILE_SITE_CCACHE)/plotutils.

pkgguilesitedir   = $(GUILE_SITE)/plotutils
pkgguileobjectdir = $(GUILE_SITE_CCACHE)/plotutils

# ------------------------------------------------------------------
# Sources to install
# ------------------------------------------------------------------

dist_pkgguilesite_DATA = \
  plot.scm \
  graph.scm

# ------------------------------------------------------------------
# Compiled .go objects (optional, gated on guild availability)
# ------------------------------------------------------------------

if GUILD

pkgguileobject_DATA = $(dist_pkgguilesite_DATA:%.scm=%.go)

GUILEC_FLAGS = \
  -O2 \
  -Warity-mismatch \
  -Wformat \
  -Wmacro-use-before-definition \
  -Wunbound-variable \
  --load-path=$(abs_top_srcdir)/module \
  --load-path=$(abs_top_builddir)/module

.scm.go:
	GUILE_AUTO_COMPILE=0 \
	LTDL_LIBRARY_PATH=$(abs_top_builddir)/src/.libs \
	$(GUILE_TOOLS) compile --target="$(host)" $(GUILEC_FLAGS) \
	  -o "$@" "$<"

# Compiling .scm to .go loads the extension library, so the library
# must be built first.  SUBDIRS ordering in the top-level Makefile.am
# is what actually guarantees this; the line below simply documents
# the prerequisite for anyone reading the Makefile.
$(pkgguileobject_DATA): $(top_builddir)/src/libguile-plotutils.la

CLEANFILES = $(pkgguileobject_DATA)

endif
