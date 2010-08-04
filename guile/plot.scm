(define-module (plotutils plot)
  #:export (
	    alabel!
	    arc!
	    arcrel!
	    bezier2!
	    bezier2rel!
	    bezier3!
	    bezier3rel!
	    bgcolor!
	    bgcolorname!
	    box!
	    boxrel!
	    capmod!
	    circle!
	    circlerel!
	    closepl!
	    color!
	    colorname!
	    concat!
	    cont!
	    contrel!
	    ellarc!
	    ellarcrel!
	    ellipse!
	    ellipserel!
	    endpath!
	    endsubpath!
	    erase!
	    fillcolor!
	    fillcolorname!
	    fillmod!
	    filltype!
	    flushpl!
	    fontname!
	    fontsize!
	    havecap
	    joinmod!
	    label!
	    labelwidth!
	    line!
	    linerel!
	    linewidth!
	    marker!
	    markerrel!
	    miterlimit!
	    move!
	    moverel!
	    newpl
	    newplparams
	    openpl!
	    orientation!
	    pencolor!
	    pencolorname!
	    pentype!
	    plotter?
	    plparams?
	    point!
	    pointrel!
	    restorestate!
	    rotate!
	    savestate!
	    scale!
	    setmatrix!
	    setplparam!
	    space!
	    space2!
	    textangle!
	    translate!
	    ))

(define (alabel! plotter horiz-justify vert-justify str)
  (%alabel! plotter
	    (char->integer (string-ref (symbol->string horiz-justify) 0))
	    (char->integer (string-ref (symbol->string vert-justify) 0))
	    str))

(load-extension "libguile-plotutils" "gupl_plot_init")

