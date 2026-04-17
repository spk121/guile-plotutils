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

(define (draw-histogram-bars! plotter bins bin-width mu sigma sample-size)
  "Draw filled histogram bars using Gaussian expected bin mass."
  (fillcolorname! plotter "lightsteelblue")
  (pencolorname! plotter "midnightblue")
  (filltype! plotter 1)
  (linewidth! plotter 0.08)
  (for-each
   (lambda (left)
     (let* ((right (+ left bin-width))
            (center (+ left (/ bin-width 2.0)))
            ;; Approximate bin count via midpoint rule.
            (height (* sample-size bin-width (gaussian-pdf center mu sigma))))
       (box! plotter left 0.0 right height)))
   bins)
  (filltype! plotter 0))

(define (draw-axes! plotter x-min x-max y-max)
  "Draw x/y axes with tick marks and numeric labels."
  (pencolorname! plotter "black")
  (linewidth! plotter 0.12)

  ;; Axes lines
  (line! plotter x-min 0.0 x-max 0.0)
  (line! plotter x-min 0.0 x-min y-max)

  ;; Tick styling text
  (fontname! plotter "HersheySerif")
  (fontsize! plotter 1.5)

  ;; X-axis ticks and labels at integer points
  (for-each
   (lambda (x)
     (line! plotter x -1.5 x 1.5)
     (move! plotter x -4.0)
     (alabel! plotter 'c 't (format #f "~a" (inexact->exact (round x)))))
   (frange -4.0 4.1 1.0))

  ;; Y-axis ticks and labels every 20 units
  (for-each
   (lambda (y)
     (line! plotter (- x-min 0.08) y (+ x-min 0.08) y)
     (move! plotter (- x-min 0.25) y)
     (alabel! plotter 'r 'c (format #f "~a" (inexact->exact (round y)))))
   (frange 0.0 (+ y-max 0.1) 20.0)))

(define (draw-labels! plotter x-min x-max y-max)
  "Draw axis labels and figure title."
  (pencolorname! plotter "black")

  ;; Title
  (fontname! plotter "HersheySerif")
  (fontsize! plotter 2.2)
  (move! plotter (/ (+ x-min x-max) 2.0) (+ y-max 8.0))
  (alabel! plotter 'c 'c "Gaussian Distribution Histogram (bin width = 0.25)")

  ;; X-axis label
  (fontsize! plotter 1.8)
  (move! plotter (/ (+ x-min x-max) 2.0) -10.0)
  (alabel! plotter 'c 'c "x")

  ;; Y-axis label (rotated)
  (textangle! plotter 90.0)
  (move! plotter (- x-min 0.95) (/ y-max 2.0))
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

    (setplparam! param "BITMAPSIZE" "900x600")

    (let ((plotter (newpl output-format fp (current-error-port) param)))
      (openpl! plotter)

      ;; Leave margins for axis labels and title.
      (space! plotter -4.8 -12.0 4.8 140.0)
      (erase! plotter)
      (bgcolorname! plotter "white")

      (draw-histogram-bars! plotter bins bin-width mu sigma sample-size)
      (draw-axes! plotter x-min x-max y-max)
      (draw-labels! plotter x-min x-max y-max)

      (closepl! plotter)
      (close fp))))

(main (command-line))
