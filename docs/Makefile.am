info_TEXINFOS = guile-plotutils.texi fdl-1.3.texi

# Avoid automake's in-srcdir texinfo regeneration in read-only distcheck
# trees; docs are still available via explicit info/html targets.
INFO_DEPS =

# Prefer using HTML as our distcheck documentation target, rather than
# DVI, because for some of our automated builds, downloading texlive
# or basictex takes a fair amount of time.
AM_DISTCHECK_DVI_TARGET = html

# ------------------------------------------------------------------
# Plots
# ------------------------------------------------------------------
PLOTS = \
 graph-sine-wave \
 graph-bessel \
 graph-chebyshev \
 graph-damped-oscillator \
 graph-elliptic-integral \
 graph-fourier \
 graph-fresnel \
 graph-gamma \
 graph-gaussian \
 graph-lissajous \
 graph-logistic-growth \
 graph-logistic \
 graph-riemann-stairs \
 graph-weierstrass \
 graph-zeta \
 graph-sine-wave2 \
 plot-dragon-curve \
 plot-gaussian-histogram \
 plot-golden-spiral \
 plot-hilbert-curve \
 plot-koch-snowflake \
 plot-sierpinski-triangle \
 plot-spirograph
 
# ------------------------------------------------------------------
# Generated images
# ------------------------------------------------------------------
IMAGES_PNG = $(addprefix images/,$(PLOTS:=.png))
IMAGES_EPS = $(addprefix images/,$(PLOTS:=.eps))

# Extra files to distribute
EXTRA_DIST = \
	scripts/$(PLOTS:=.scm) \
	document-1.0.1.css

# Automake's texinfo dependency scanner may record verbatiminclude files
# as bare basenames for distdir. Provide a build-time mapping from docs/
# to docs/scripts/ so VPATH/distcheck can satisfy those dependencies.
SCM_BASENAMES = $(addsuffix .scm,$(PLOTS))

$(SCM_BASENAMES): %.scm: scripts/%.scm
	cp -f $(srcdir)/scripts/$*.scm $@

CLEANFILES = $(SCM_BASENAMES)

# ------------------------------------------------------------------
# Generate images from Scheme scripts
# ------------------------------------------------------------------
top-build-prereqs:
	$(MAKE) -C $(top_builddir) docs-prereqs

images/%.png images/%.eps: top-build-prereqs scripts/%.scm
	$(MKDIR_P) images
	$(AM_V_GEN)$(LIBTOOL) --mode=execute \
		env \
			GUILE_EXTENSIONS_PATH="$(top_builddir)/.libs$${GUILE_EXTENSIONS_PATH:+:$${GUILE_EXTENSIONS_PATH}}" \
			LTDL_LIBRARY_PATH="$(top_builddir)/.libs$${LTDL_LIBRARY_PATH:+:$${LTDL_LIBRARY_PATH}}" \
		$(GUILE) -L $(top_builddir)/module -L $(top_srcdir)/module \
		$(srcdir)/scripts/$*.scm $@

BUILT_SOURCES = $(IMAGES_PNG) $(IMAGES_EPS)

# ------------------------------------------------------------------
# HTML support — images + CSS
# ------------------------------------------------------------------
html-local:
	$(MKDIR_P) guile-plotutils.html/images
	cp -f $(IMAGES_PNG) guile-plotutils.html/images/ 2>/dev/null || true
	cp -f document-1.0.1.css guile-plotutils.html/ 2>/dev/null || true

# Tell texi2any to use our custom CSS file
AM_MAKEINFOHTMLFLAGS = --css-include=document-1.0.1.css

# ------------------------------------------------------------------
# Other documentation targets
# ------------------------------------------------------------------
pdf-local:
	$(MKDIR_P) images
	cp -f $(IMAGES_PNG) images/ 2>/dev/null || true

dvi-local ps-local:
	$(MKDIR_P) images
	cp -f $(IMAGES_EPS) images/ 2>/dev/null || true

info-local:
	$(MKDIR_P) images
	cp -f $(IMAGES_PNG) images/ 2>/dev/null || true

# ------------------------------------------------------------------
# Install rules
# ------------------------------------------------------------------
install-html-local:
	$(MKDIR_P) $(DESTDIR)$(docdir)/guile-plotutils/images
	cp -f $(IMAGES_PNG) $(DESTDIR)$(docdir)/guile-plotutils/images/ 2>/dev/null || true
	cp -f document-1.0.1.css $(DESTDIR)$(docdir)/guile-plotutils/ 2>/dev/null || true

install-pdf-local install-dvi-local install-ps-local install-info-local:
	$(MKDIR_P) $(DESTDIR)$(docdir)/guile-plotutils
	cp -f $(IMAGES_PNG) $(DESTDIR)$(docdir)/guile-plotutils/ 2>/dev/null || true

# ------------------------------------------------------------------
# Cleanup
# ------------------------------------------------------------------
mostlyclean-local:
	rm -rf images guile-plotutils.html/images

# Keep distributed .info files fresher than sources so distcheck does not
# attempt to regenerate them in its read-only source tree.
dist-hook: $(INFO_DEPS)

.PHONY: top-build-prereqs
