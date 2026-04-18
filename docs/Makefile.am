## docs/Makefile.am -- build the guile-plotutils manual

info_TEXINFOS = guile-plotutils.texi

# Use HTML (not DVI) for distcheck's documentation build; pulling in
# texlive/basictex in CI is slow.
AM_DISTCHECK_DVI_TARGET = html

# Custom CSS for the HTML manual.
AM_MAKEINFOHTMLFLAGS = --css-include=$(srcdir)/document-1.0.1.css

# ------------------------------------------------------------------
# Scripts and the images they generate
# ------------------------------------------------------------------
# Each script in scripts/FOO.scm generates an image when run with an
# output path argument:
#
#     guile scripts/FOO.scm images/FOO.png
#     guile scripts/FOO.scm images/FOO.eps
#
# The manual includes the scripts verbatim and embeds the images;
# .png is used for info/html/pdf, .eps is used for dvi/ps.
#
# The generated images are shipped in the distribution tarball, so
# end users can build the manual without running guile at all.  The
# rules that regenerate them are gated on MAINTAINER_MODE below.

SCM_SRC = \
	scripts/graph-sine-wave.scm \
	scripts/graph-sine-wave2.scm \
	scripts/graph-bessel.scm \
	scripts/graph-chebyshev.scm \
	scripts/graph-damped-oscillator.scm \
	scripts/graph-elliptic-integral.scm \
	scripts/graph-fourier.scm \
	scripts/graph-fresnel.scm \
	scripts/graph-gamma.scm \
	scripts/graph-gaussian.scm \
	scripts/graph-lissajous.scm \
	scripts/graph-logistic-growth.scm \
	scripts/graph-logistic.scm \
	scripts/graph-riemann-stairs.scm \
	scripts/graph-weierstrass.scm \
	scripts/graph-zeta.scm \
	scripts/plot-dragon-curve.scm \
	scripts/plot-gaussian-histogram.scm \
	scripts/plot-golden-spiral.scm \
	scripts/plot-hilbert-curve.scm \
	scripts/plot-koch-snowflake.scm \
	scripts/plot-sierpinski-triangle.scm \
	scripts/plot-spirograph.scm

IMAGES_PNG = $(SCM_SRC:scripts/%.scm=images/%.png)
HTML_IMAGES_PNG = $(SCM_SRC:scripts/%.scm=guile-plotutils.html/images/%.png)
IMAGES_EPS = $(SCM_SRC:scripts/%.scm=images/%.eps)
noinst_SCRIPTS = $(SCM_SRC)

# Files the guile-plotutils manual depends on: the included fdl
# appendix, the embedded images, and the verbatim-included scripts.
# Listing them here both distributes them in the tarball and makes
# the manual rebuild when any of them changes.
guile_plotutils_TEXINFOS = \
	fdl-1.3.texi \
	colors.txt \
	$(SCM_SRC) \
	$(IMAGES_PNG) \
	$(IMAGES_EPS)

EXTRA_DIST = document-1.0.1.css

# Texinfo's @image directive produces <img src="images/foo.png"> in
# the generated HTML.  The browser resolves that relative to the
# HTML file's directory (guile-plotutils.html/), not the Makefile's
# directory, so we have to stage the images into the HTML tree.
html-local: $(HTML_IMAGES_PNG)

guile-plotutils.html/images/%.png: images/%.png
	@mkdir -p $(dir $@)
	cp $< $@

# ------------------------------------------------------------------
# Rules to regenerate the images (maintainer only)
# ------------------------------------------------------------------
# End users build from a tarball that already contains the images, so
# they never need guile or the plotutils extension library to build
# the manual.  Only the maintainer regenerates images, via
#     ./configure --enable-maintainer-mode

if MAINTAINER_MODE

# Running a .scm from an uninstalled build needs the in-tree modules
# and the compiled extension library on the right search paths.
GUILE_RUN = \
	$(LIBTOOL) --mode=execute \
		env \
			GUILE_EXTENSIONS_PATH="$(abs_top_builddir)/src/.libs$${GUILE_EXTENSIONS_PATH:+:$${GUILE_EXTENSIONS_PATH}}" \
			LTDL_LIBRARY_PATH="$(abs_top_builddir)/src/.libs$${LTDL_LIBRARY_PATH:+:$${LTDL_LIBRARY_PATH}}" \
		$(GUILE) \
			-L $(abs_top_builddir)/module \
			-L $(abs_top_srcdir)/module

images/%.png: scripts/%.scm
	$(AM_V_GEN)$(MKDIR_P) images
	$(AM_V_at)$(GUILE_RUN) $< $@

images/%.eps: scripts/%.scm
	$(AM_V_GEN)$(MKDIR_P) images
	$(AM_V_at)$(GUILE_RUN) $< $@

# The images are shipped artifacts, so only `make maintainer-clean`
# removes them.  A plain `make clean` in an end-user build must not
# delete files that came from the tarball.
MAINTAINERCLEANFILES = $(IMAGES_PNG) $(IMAGES_EPS)

maintainer-clean-local:
	-rm -rf images

endif
