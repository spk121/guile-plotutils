;;; ex-graph-fresnel.scm — Fresnel integrals S(x) and C(x)
;;; S(x) = ∫₀ˣ sin(π/2·t²) dt,  C(x) = ∫₀ˣ cos(π/2·t²) dt
;;; These arise in diffraction theory (Fresnel diffraction at a straight
;;; edge).  Both oscillate and converge to ½ as x → ∞.  Also plots the
;;; Cornu (Euler) spiral: the parametric curve (C(t), S(t)).

(use-modules (srfi srfi-1)
             (plotutils graph))

(define pi (* 4.0 (atan 1.0)))

(define (integrate-simpson f a b n)
  "Numerical integration of f from a to b using Simpson's rule
with n subintervals."
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

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-cornu-spiral.svg"))
         (output-format (output-format-from-filename output-file))
         ;; Cornu spiral: parametric (C(t), S(t)) for t in [-7, 7]
         (nt 600)
         (tmin -7.0)
         (tmax 7.0)
         (tstep (/ (- tmax tmin) nt))
         (ts (iota nt tmin tstep))
         (spiral-x (map fresnel-c ts))
         (spiral-y (map fresnel-s ts)))
    ;; Cornu (Euler) spiral
    (with-output-to-file output-file
      (lambda ()
        (graph spiral-x spiral-y
               #:output-format output-format
               #:top-label "Cornu (Euler) Spiral"
               #:x-label "C(t)"
               #:y-label "S(t)"
               #:x-limits '(-1.0 1.0)
               #:y-limits '(-1.0 1.0)
               #:grid-style 3
               #:line-width 0.003
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
