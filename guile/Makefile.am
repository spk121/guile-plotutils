## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libguile-plotutils.la

libguile_plotutils_la_SOURCES = guile_plot.h guile_plot.c	\
	guile_graph.h guile_graph.c visibility.h

# Sources from the graph program
libguile_plotutils_la_SOURCES += ../graph/misc.c ../graph/linemode.c	\
	../graph/reader.c ../graph/plotter.c ../graph/fontlist.c

# Sources from the lib directory
libguile_plotutils_la_SOURCES += ../lib/compat.c ../lib/display.c	\
	../lib/getopt.c ../lib/getopt1.c ../lib/xmalloc.c		\
	../lib/xstrdup.c

libguile_plotutils_la_CPPFLAGS = $(GUILE_CFLAGS) \
	-I$(srcdir)/../include -I$(srcdir)/../graph

libguile_plotutils_la_LDFLAGS = -module -no-undefined -version-info 1:0:0 \
	$(GUILE_LDFLAGS) -Xlinker -Map=output.map

libguile_plotutils_la_LIBADD = ../libplot/libplot.la -lguile

guilesitedir = ${datadir}/guile/site
pkgguilesitedir = $(guilesitedir)/$(PACKAGE)
dist_pkgguilesite_DATA = plot.scm graph.scm
ETAGS_ARGS = plot.scm graph.scm
