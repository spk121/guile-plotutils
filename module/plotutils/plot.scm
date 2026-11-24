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
	    closepath!
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
	    deletepl!
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
	    linedash!
	    list-fonts
	    linemod!
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
	    plotter-fonts
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

;;; The built-in fonts of libplot, as documented in its manual (and in
;;; the Text Fonts appendix of this one), in the order of GNU graph
;;; --list-fonts.  Each entry is (NAME . CLASS).  "Courier",
;;; "Courier-Bold" and "Symbol" appear twice, as PCL 5 and as Postscript
;;; fonts.
(define plotter-fonts
  '(
    ;; Hershey vector fonts
    ("HersheySerif" . hershey)
    ("HersheySerif-Italic" . hershey)
    ("HersheySerif-Bold" . hershey)
    ("HersheySerif-BoldItalic" . hershey)
    ("HersheyCyrillic" . hershey)
    ("HersheyCyrillic-Oblique" . hershey)
    ("HersheyEUC" . hershey)
    ("HersheySans" . hershey)
    ("HersheySans-Oblique" . hershey)
    ("HersheySans-Bold" . hershey)
    ("HersheySans-BoldOblique" . hershey)
    ("HersheyScript" . hershey)
    ("HersheyScript-Bold" . hershey)
    ("HersheyGothicEnglish" . hershey)
    ("HersheyGothicGerman" . hershey)
    ("HersheyGothicItalian" . hershey)
    ("HersheySerifSymbol" . hershey)
    ("HersheySerifSymbol-Oblique" . hershey)
    ("HersheySerifSymbol-Bold" . hershey)
    ("HersheySerifSymbol-BoldOblique" . hershey)
    ("HersheySansSymbol" . hershey)
    ("HersheySansSymbol-Oblique" . hershey)
    ;; HP vector fonts; the "extra" ones are for HP-GL/2 output only
    ("Arc" . hp-vector)
    ("Arc-Oblique" . hp-vector)
    ("Arc-Bold" . hp-vector)
    ("Arc-BoldOblique" . hp-vector)
    ("ArcANK" . hp-vector-extra)
    ("ArcANK-Oblique" . hp-vector-extra)
    ("ArcANK-Bold" . hp-vector-extra)
    ("ArcANK-BoldOblique" . hp-vector-extra)
    ("ArcSymbol" . hp-vector-extra)
    ("Stick" . hp-vector)
    ("Stick-Oblique" . hp-vector)
    ("Stick-Bold" . hp-vector)
    ("Stick-BoldOblique" . hp-vector)
    ("StickANK" . hp-vector-extra)
    ("StickANK-Oblique" . hp-vector-extra)
    ("StickANK-Bold" . hp-vector-extra)
    ("StickANK-BoldOblique" . hp-vector-extra)
    ("StickSymbol" . hp-vector-extra)
    ;; PCL 5 fonts
    ("Univers" . pcl)
    ("Univers-Oblique" . pcl)
    ("Univers-Bold" . pcl)
    ("Univers-BoldOblique" . pcl)
    ("UniversCondensed" . pcl)
    ("UniversCondensed-Oblique" . pcl)
    ("UniversCondensed-Bold" . pcl)
    ("UniversCondensed-BoldOblique" . pcl)
    ("CGTimes" . pcl)
    ("CGTimes-Italic" . pcl)
    ("CGTimes-Bold" . pcl)
    ("CGTimes-BoldItalic" . pcl)
    ("AlbertusMedium" . pcl)
    ("AlbertusExtraBold" . pcl)
    ("AntiqueOlive" . pcl)
    ("AntiqueOlive-Italic" . pcl)
    ("AntiqueOlive-Bold" . pcl)
    ("Arial-Roman" . pcl)
    ("Arial-Italic" . pcl)
    ("Arial-Bold" . pcl)
    ("Arial-BoldItalic" . pcl)
    ("ClarendonCondensed" . pcl)
    ("Coronet" . pcl)
    ("Courier" . pcl)
    ("Courier-Italic" . pcl)
    ("Courier-Bold" . pcl)
    ("Courier-BoldItalic" . pcl)
    ("Garamond" . pcl)
    ("Garamond-Italic" . pcl)
    ("Garamond-Bold" . pcl)
    ("Garamond-BoldItalic" . pcl)
    ("LetterGothic" . pcl)
    ("LetterGothic-Italic" . pcl)
    ("LetterGothic-Bold" . pcl)
    ("Marigold" . pcl)
    ("CGOmega" . pcl)
    ("CGOmega-Italic" . pcl)
    ("CGOmega-Bold" . pcl)
    ("CGOmega-BoldItalic" . pcl)
    ("TimesNewRoman" . pcl)
    ("TimesNewRoman-Italic" . pcl)
    ("TimesNewRoman-Bold" . pcl)
    ("TimesNewRoman-BoldItalic" . pcl)
    ("Wingdings" . pcl)
    ("Symbol" . pcl)
    ;; Postscript fonts
    ("Helvetica" . ps)
    ("Helvetica-Oblique" . ps)
    ("Helvetica-Bold" . ps)
    ("Helvetica-BoldOblique" . ps)
    ("Helvetica-Narrow" . ps)
    ("Helvetica-Narrow-Oblique" . ps)
    ("Helvetica-Narrow-Bold" . ps)
    ("Helvetica-Narrow-BoldOblique" . ps)
    ("Times-Roman" . ps)
    ("Times-Italic" . ps)
    ("Times-Bold" . ps)
    ("Times-BoldItalic" . ps)
    ("AvantGarde-Book" . ps)
    ("AvantGarde-BookOblique" . ps)
    ("AvantGarde-Demi" . ps)
    ("AvantGarde-DemiOblique" . ps)
    ("Bookman-Light" . ps)
    ("Bookman-LightItalic" . ps)
    ("Bookman-Demi" . ps)
    ("Bookman-DemiItalic" . ps)
    ("Courier" . ps)
    ("Courier-Oblique" . ps)
    ("Courier-Bold" . ps)
    ("Courier-BoldOblique" . ps)
    ("NewCenturySchlbk-Roman" . ps)
    ("NewCenturySchlbk-Italic" . ps)
    ("NewCenturySchlbk-Bold" . ps)
    ("NewCenturySchlbk-BoldItalic" . ps)
    ("Palatino-Roman" . ps)
    ("Palatino-Italic" . ps)
    ("Palatino-Bold" . ps)
    ("Palatino-BoldItalic" . ps)
    ("ZapfChancery-MediumItalic" . ps)
    ("ZapfDingbats" . ps)
    ("Symbol" . ps)
    ))

;; The libplot capability (see havecap) for each class of font
(define font-class-capabilities
  '((hershey . "HERSHEY_FONTS")
    (hp-vector . "STICK_FONTS")
    (hp-vector-extra . "STICK_FONTS")
    (pcl . "PCL_FONTS")
    (ps . "PS_FONTS")))

(define (list-fonts output-format)
  "Return a list of the names of the fonts that Plotters of type
OUTPUT-FORMAT, a string such as \"svg\", support."
  (unless (string? output-format)
    (scm-error 'wrong-type-arg "list-fonts"
               "Wrong type argument in position ~A: ~S"
               (list 1 output-format) (list output-format)))
  (let ((answers
         (map (lambda (class+cap)
                (cons (car class+cap)
                      (%format-havecap output-format (cdr class+cap))))
              font-class-capabilities)))
    (define (supported? class)
      (and (not (zero? (assq-ref answers class)))
           ;; The extra HP vector fonts are documented as available only
           ;; for HP-GL/2 output, which an "hpgl" Plotter produces by
           ;; default.  A Metafile Plotter accepts every font, as plot(1)
           ;; may render the metafile in any output format.
           (or (not (eq? class 'hp-vector-extra))
               (string-ci=? output-format "hpgl")
               (string-ci=? output-format "meta"))))
    (let loop ((fonts plotter-fonts) (names '()))
      (if (null? fonts)
          (reverse! names)
          (loop (cdr fonts)
                (if (supported? (cdar fonts))
                    (cons (caar fonts) names)
                    names))))))
