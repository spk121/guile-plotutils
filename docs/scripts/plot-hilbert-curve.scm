;;; ex-plot-hilbert-curve.scm — Hilbert space-filling curve
;;; The Hilbert curve is a continuous fractal space-filling curve
;;; first described by David Hilbert in 1891.  At each level of
;;; recursion it visits every cell of a 2^n × 2^n grid exactly once.
;;; In the limit, it maps the unit interval [0,1] surjectively onto the
;;; unit square [0,1]², providing a continuous mapping from 1D to 2D.

(use-modules (plotutils plot))

(define pi (* 4.0 (atan 1.0)))

(define *plotter* #f)
(define *angle* 0.0)
(define *x* 0.0)
(define *y* 0.0)

(define (turtle-forward! dist)
  (let ((nx (+ *x* (* dist (cos (* *angle* (/ pi 180.0))))))
        (ny (+ *y* (* dist (sin (* *angle* (/ pi 180.0)))))))
    (cont! *plotter* nx ny)
    (set! *x* nx)
    (set! *y* ny)))

(define (turtle-left! degrees)
  (set! *angle* (+ *angle* degrees)))

(define (turtle-right! degrees)
  (set! *angle* (- *angle* degrees)))

(define (hilbert level size parity)
  "Draw a Hilbert curve of given level. PARITY is +1 or -1."
  (when (> level 0)
    (turtle-left! (* parity 90))
    (hilbert (- level 1) size (- parity))
    (turtle-forward! size)
    (turtle-right! (* parity 90))
    (hilbert (- level 1) size parity)
    (turtle-forward! size)
    (hilbert (- level 1) size parity)
    (turtle-right! (* parity 90))
    (turtle-forward! size)
    (hilbert (- level 1) size (- parity))
    (turtle-left! (* parity 90))))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "plot-hilbert-curve.svg"))
         (output-format (output-format-from-filename output-file))
         (fp (open-output-file output-file #:binary #t))
         (param (newplparams)))
    (setplparam! param "BITMAPSIZE" "800x800")
    (let* ((plotter (newpl output-format fp (current-error-port) param))
           (order 6)
           (cells (- (expt 2 order) 1))
           (step (/ 760.0 cells)))
      (set! *plotter* plotter)
      (openpl! plotter)
      (space! plotter -20.0 -20.0 820.0 820.0)
      (linewidth! plotter 0.5)
      (erase! plotter)
      (pencolorname! plotter "dark orchid")

      ;; Start at lower left
      (set! *x* 20.0)
      (set! *y* 20.0)
      (set! *angle* 0.0)
      (move! plotter *x* *y*)

      (hilbert order step 1)

      (closepl! plotter)
      (close fp))))

(main (command-line))
