;;; ex-plot-gaussian-histogram.scm — Gaussian histogram with filled bins
;;; Uses (plotutils plot) primitives to draw a histogram of a normal PDF
;;; with fixed bin width 0.25, including axes, ticks, labels, and title.

(use-modules (plotutils plot)
             (srfi srfi-1)
             (ice-9 format))

(define pi (* 4.0 (atan 1.0)))

(define (gaussian-pdf x mu sigma)
  (/ (exp (- (/ (* (- x mu) (- x mu)) (* 2.0 sigma sigma))))
     (* sigma (sqrt (* 2.0 pi)))))

(define (frange start stop step)
  "Create a numeric range [start, stop) with uniform step."
  (let loop ((x start) (acc '()))
    (if (>= x stop)
        (reverse acc)
        (loop (+ x step) (cons x acc)))))

;; The drawing uses a square 1000 x 1000 page (see main).  libplot draws
;; text in user coordinates, so a user space with unequal x and y scales
;; would stretch every label; instead, data coordinates (x, count) are
;; mapped onto this plotting area of the page.
(define plot-left 140.0)
(define plot-right 950.0)
(define plot-bottom 280.0)
(define plot-top 760.0)

(define (make-page-x x-min x-max)
  (lambda (x)
    (+ plot-left (* (- plot-right plot-left) (/ (- x x-min) (- x-max x-min))))))

(define (make-page-y y-max)
  (lambda (y)
    (+ plot-bottom (* (- plot-top plot-bottom) (/ y y-max)))))

(define (draw-histogram-bars! plotter px py bins bin-width mu sigma sample-size)
  "Draw filled histogram bars using Gaussian expected bin mass."
  (fillcolorname! plotter "lightsteelblue")
  (pencolorname! plotter "midnightblue")
  (filltype! plotter 1)
  (linewidth! plotter 1.0)
  (for-each
   (lambda (left)
     (let* ((right (+ left bin-width))
            (center (+ left (/ bin-width 2.0)))
            ;; Approximate bin count via midpoint rule.
            (height (* sample-size bin-width (gaussian-pdf center mu sigma))))
       (box! plotter (px left) (py 0.0) (px right) (py height))))
   bins)
  (filltype! plotter 0))

(define (draw-axes! plotter px py x-min x-max y-max)
  "Draw x/y axes with tick marks and numeric labels."
  (pencolorname! plotter "black")
  (linewidth! plotter 2.0)

  ;; Axes lines
  (line! plotter (px x-min) (py 0.0) (px x-max) (py 0.0))
  (line! plotter (px x-min) (py 0.0) (px x-min) (py y-max))

  ;; Tick styling text
  (fontname! plotter "HersheySerif")
  (fontsize! plotter 24.0)

  ;; X-axis ticks and labels at integer points
  (for-each
   (lambda (x)
     (line! plotter (px x) (- (py 0.0) 10.0) (px x) (py 0.0))
     (move! plotter (px x) (- (py 0.0) 20.0))
     (alabel! plotter 'c 't (format #f "~a" (inexact->exact (round x)))))
   (frange x-min (+ x-max 0.1) 1.0))

  ;; Y-axis ticks and labels every 20 units
  (for-each
   (lambda (y)
     (line! plotter (- (px x-min) 10.0) (py y) (px x-min) (py y))
     (move! plotter (- (px x-min) 18.0) (py y))
     (alabel! plotter 'r 'c (format #f "~a" (inexact->exact (round y)))))
   (frange 0.0 (+ y-max 0.1) 20.0)))

(define (draw-labels! plotter px py x-min x-max y-max)
  "Draw axis labels and figure title."
  (pencolorname! plotter "black")
  (fontname! plotter "HersheySerif")

  ;; Title
  (fontsize! plotter 30.0)
  (move! plotter 500.0 (+ plot-top 70.0))
  (alabel! plotter 'c 'c "Gaussian Distribution Histogram (bin width = 0.25)")

  ;; X-axis label
  (fontsize! plotter 30.0)
  (move! plotter (/ (+ plot-left plot-right) 2.0) (- plot-bottom 80.0))
  (alabel! plotter 'c 'c "x")

  ;; Y-axis label (rotated)
  (textangle! plotter 90.0)
  (move! plotter (- plot-left 95.0) (/ (+ plot-bottom plot-top) 2.0))
  (alabel! plotter 'c 'c "Frequency")
  (textangle! plotter 0.0))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "plot-gaussian-histogram.svg"))
         (output-format (output-format-from-filename output-file))
         (fp (open-output-file output-file #:binary #t))
         (param (newplparams))
         (bin-width 0.25)
         (x-min -4.0)
         (x-max 4.0)
         (y-max 130.0)
         (mu 0.0)
         (sigma 1.0)
         (sample-size 1000.0)
         (bins (frange x-min x-max bin-width)))

    ;; Square, like the user space below, so nothing is stretched.
    (setplparam! param "BITMAPSIZE" "900x900")

    (let ((plotter (newpl output-format fp (current-error-port) param)))
      (openpl! plotter)

      ;; A square page; the histogram is mapped onto part of it.
      (space! plotter 0.0 0.0 1000.0 1000.0)
      (erase! plotter)
      (bgcolorname! plotter "white")

      (let ((px (make-page-x x-min x-max))
            (py (make-page-y y-max)))
        (draw-histogram-bars! plotter px py bins bin-width mu sigma sample-size)
        (draw-axes! plotter px py x-min x-max y-max)
        (draw-labels! plotter px py x-min x-max y-max))

      (closepl! plotter)
      ;; Delete the plotter before closing the port: Postscript (.eps)
      ;; output is written only when the plotter is deleted.
      (deletepl! plotter)
      (close fp))))

(main (command-line))
