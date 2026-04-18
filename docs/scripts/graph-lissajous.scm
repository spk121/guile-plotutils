;;; ex-graph-lissajous.scm — Lissajous figures (parametric curves)
;;; x(t) = sin(a·t + δ), y(t) = sin(b·t)
;;; These curves arise in physics when two perpendicular harmonic
;;; oscillations are superposed.  The ratio a:b determines the topology.

(use-modules (srfi srfi-1)
             (plotutils graph))

(define pi (* 4.0 (atan 1.0)))

(define (lissajous a b delta n)
  "Return (xlist . ylist) for the Lissajous figure with parameters a, b, delta."
  (let* ((step (/ (* 2.0 pi) n))
         (ts (iota n 0.0 step))
         (xs (map (lambda (t) (sin (+ (* a t) delta))) ts))
         (ys (map (lambda (t) (sin (* b t))) ts)))
    (cons xs ys)))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-lissajous.svg"))
         (output-format (output-format-from-filename output-file))
         (n 1000)
         (fig1 (lissajous 3 2 (/ pi 4) n))    ; 3:2
         (fig2 (lissajous 5 4 (/ pi 2) n))    ; 5:4
         (fig3 (lissajous 3 4 (/ pi 3) n)))   ; 3:4
    (with-output-to-file output-file
      (lambda ()
        (graph (merge (car fig1) (car fig2) (car fig3))
               (merge (cdr fig1) (cdr fig2) (cdr fig3))
               #:output-format output-format
               #:bitmap-size "1000x1000"
               #:top-label "Lissajous Figures (3:2, 5:4, 3:4)"
               #:x-label "x(t) = sin(a t + \\*d)"
               #:y-label "y(t) = sin(b t)"
               #:x-limits '(-1.2 1.2)
               #:y-limits '(-1.2 1.2)
               #:toggle-use-color #t
               #:grid-style 2
               #:line-width 0.003
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
