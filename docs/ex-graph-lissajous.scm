#!/usr/bin/guile \
-e main -s
!#
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

(define (main args)
  (let* ((n 1000)
         (fig1 (lissajous 3 2 (/ pi 4) n))    ; 3:2
         (fig2 (lissajous 5 4 (/ pi 2) n))    ; 5:4
         (fig3 (lissajous 3 4 (/ pi 3) n)))   ; 3:4
    (with-output-to-file "graph-lissajous.svg"
      (lambda ()
        (graph (merge (car fig1) (car fig2) (car fig3))
               (merge (cdr fig1) (cdr fig2) (cdr fig3))
               #:output-format "svg"
               #:top-label "Lissajous Figures (3:2, 5:4, 3:4)"
               #:x-label "x(t) = sin(a t + \\delta)"
               #:y-label "y(t) = sin(b t)"
               #:x-limits '(-1.2 1.2)
               #:y-limits '(-1.2 1.2)
               #:toggle-use-color #t
               #:grid-style 2
               #:line-width 0.003
               #:font-name "HersheySerif"))
      #:binary #t)))
