;;; ex-plot-koch-snowflake.scm — Koch Snowflake fractal
;;; The Koch snowflake is constructed by starting with an equilateral
;;; triangle and recursively replacing the middle third of each edge
;;; with two sides of a smaller equilateral triangle.  Its perimeter
;;; is infinite while enclosing finite area.  Hausdorff dimension = log4/log3 ≈ 1.2619.

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

(define (koch-segment length depth)
  "Draw one Koch curve segment of given length at given recursion depth."
  (if (= depth 0)
      (turtle-forward! length)
      (begin
        (koch-segment (/ length 3.0) (- depth 1))
        (turtle-left! 60)
        (koch-segment (/ length 3.0) (- depth 1))
        (turtle-right! 120)
        (koch-segment (/ length 3.0) (- depth 1))
        (turtle-left! 60)
        (koch-segment (/ length 3.0) (- depth 1)))))

(define (koch-snowflake side depth)
  "Draw a complete Koch snowflake (3 Koch curve segments)."
  (koch-segment side depth)
  (turtle-right! 120)
  (koch-segment side depth)
  (turtle-right! 120)
  (koch-segment side depth))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "plot-koch-snowflake.svg"))
         (output-format (output-format-from-filename output-file))
         (fp (open-output-file output-file #:binary #t))
         (param (newplparams)))
    (setplparam! param "BITMAPSIZE" "800x800")
    (let ((plotter (newpl output-format fp (current-error-port) param)))
      (set! *plotter* plotter)
      (openpl! plotter)
      (space! plotter -50.0 -50.0 850.0 850.0)
      (linewidth! plotter 0.5)
      (erase! plotter)
      (bgcolorname! plotter "white")
      (pencolorname! plotter "navy")

      ;; Position at top-left of the snowflake
      (set! *x* 100.0)
      (set! *y* 650.0)
      (set! *angle* -60.0)
      (move! plotter *x* *y*)

      (koch-snowflake 600.0 5)

      (closepl! plotter)
      (close fp))))

(main (command-line))
