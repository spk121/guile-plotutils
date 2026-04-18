## src/Makefile.am -- build libguile-plotutils.la

# ------------------------------------------------------------------
# Install location
# ------------------------------------------------------------------
# The extension library is loaded at runtime by the Guile modules
# via load-extension, so it must land in Guile's extension directory.

guileextensiondir = $(GUILE_EXTENSION)

# ------------------------------------------------------------------
# The library
# ------------------------------------------------------------------

guileextension_LTLIBRARIES = libguile-plotutils.la

libguile_plotutils_la_c_sources = \
  compat.c \
  display.c \
  fontlist.c \
  guile_graph.c \
  guile_plot.c \
  linemode.c \
  misc.c \
  plotter.c \
  reader.c \
  xmalloc.c \
  xstrdup.c

libguile_plotutils_la_internal_headers = \
  extern.h \
  fontlist.h \
  guile_graph.h \
  guile_plot.h \
  libcommon.h \
  plot.h \
  sys-defines.h \
  visibility.h

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

# ------------------------------------------------------------------
# Developer conveniences
# ------------------------------------------------------------------
# `make check-syntax` and `make indent` operate on the library
# sources and belong next to them.

check-syntax:
	$(CC) -std=c11 $(GUILE_CFLAGS) $(GLIB_CFLAGS) $(GOBJECT_CFLAGS) \
	  $(GOBJECT_INTROSPECTION_CFLAGS) $(FFI_CFLAGS) \
	  -DG_LOG_DOMAIN=\"GuileGI\" $(CFLAGS) -fsyntax-only \
	  $(libguile_plotutils_la_c_sources)

indent:
	VERSION_CONTROL=none indent $(libguile_plotutils_la_SOURCES)

.PHONY: check-syntax indent
