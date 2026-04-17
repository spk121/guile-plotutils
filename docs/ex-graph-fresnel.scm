#!/usr/bin/guile \
-e main -s
!#
;;; ex-graph-fresnel.scm — Fresnel integrals S(x) and C(x)
;;; S(x) = ∫₀ˣ sin(π/2·t²) dt,  C(x) = ∫₀ˣ cos(π/2·t²) dt
;;; These arise in diffraction theory (Fresnel diffraction at a straight
;;; edge).  Both oscillate and converge to ½ as x → ∞.  Also plots the
;;; Cornu (Euler) spiral: the parametric curve (C(t), S(t)).

(use-modules (srfi srfi-1)
             (plotutils graph))

(define pi (* 4.0 (atan 1.0)))

(define (integrate-simpson f a b n)
  "Numerical integration of f from a to b using Simpson's rule with n subintervals."
  (let* ((h (/ (- b a) n))
         (sum (+ (f a) (f b))))
    (let loop ((i 1) (s sum))
      (if (>= i n)
          (* (/ h 3.0) s)
          (let* ((x (+ a (* i h)))
                 (w (if (even? i) 2.0 4.0)))
            (loop (+ i 1) (+ s (* w (f x)))))))))

(define (fresnel-s x)
  (if (= x 0.0) 0.0
      (integrate-simpson (lambda (t) (sin (* (/ pi 2.0) t t)))
                         0.0 x 200)))

(define (fresnel-c x)
  (if (= x 0.0) 0.0
      (integrate-simpson (lambda (t) (cos (* (/ pi 2.0) t t)))
                         0.0 x 200)))

(define (main args)
  (let* ((n 400)
         (xmin 0.0)
         (xmax 7.0)
         (step (/ (- xmax xmin) n))
         (xs (iota n xmin step))
         (ys-s (map fresnel-s xs))
         (ys-c (map fresnel-c xs))
         ;; Cornu spiral: parametric (C(t), S(t)) for t in [-7, 7]
         (nt 600)
         (tmin -7.0)
         (tmax 7.0)
         (tstep (/ (- tmax tmin) nt))
         (ts (iota nt tmin tstep))
         (spiral-x (map fresnel-c ts))
         (spiral-y (map fresnel-s ts)))
    ;; Plot 1: S(x) and C(x)
    (with-output-to-file "graph-fresnel.svg"
      (lambda ()
        (graph (merge xs xs)
               (merge ys-s ys-c)
               #:output-format "svg"
               #:top-label "Fresnel Integrals S(x) and C(x)"
               #:x-label "x"
               #:y-label "S(x), C(x)"
               #:x-limits '(0.0 7.0)
               #:y-limits '(-0.2 1.0)
               #:toggle-use-color #t
               #:grid-style 3
               #:line-width 0.004
               #:font-name "HersheySerif"))
      #:binary #t)
    ;; Plot 2: Cornu (Euler) spiral
    (with-output-to-file "graph-cornu-spiral.svg"
      (lambda ()
        (graph spiral-x spiral-y
               #:output-format "svg"
               #:top-label "Cornu (Euler) Spiral"
               #:x-label "C(t)"
               #:y-label "S(t)"
               #:x-limits '(-1.0 1.0)
               #:y-limits '(-1.0 1.0)
               #:grid-style 3
               #:line-width 0.003
               #:font-name "HersheySerif"))
      #:binary #t)))
