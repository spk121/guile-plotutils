;;; ex-graph-damped-oscillator.scm — Damped harmonic oscillator
;;; x(t) = A·e^(−γt)·cos(ωt − φ)
;;; The fundamental solution to the second-order ODE of a mass-spring-damper
;;; system.  Shows underdamped, critically damped, and overdamped regimes.

(use-modules (srfi srfi-1)
             (plotutils graph))

(define pi (* 4.0 (atan 1.0)))

(define (underdamped gamma omega)
  (lambda (t)
    (* (exp (* (- gamma) t)) (cos (* omega t)))))

(define (critically-damped gamma)
  (lambda (t)
    (* (+ 1.0 (* gamma t)) (exp (* (- gamma) t)))))

(define (overdamped r1 r2)
  "Overdamped: x(t) = A·e^(r1·t) + B·e^(r2·t), both r1,r2 < 0."
  (lambda (t)
    (+ (* 0.5 (exp (* r1 t)))
       (* 0.5 (exp (* r2 t))))))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-damped-oscillator.svg"))
         (output-format (output-format-from-filename output-file))
         (n 500)
         (tmin 0.0)
         (tmax 10.0)
         (step (/ (- tmax tmin) n))
         (ts (iota n tmin step))
         ;; Underdamped: γ=0.3, ω=2π·0.5
         (y-under (map (underdamped 0.3 (* 2.0 pi 0.5)) ts))
         ;; Critically damped: γ=1.0
         (y-crit (map (critically-damped 1.0) ts))
         ;; Overdamped: roots r1=-0.5, r2=-3.0
         (y-over (map (overdamped -0.5 -3.0) ts))
         ;; Envelope for underdamped
         (y-env+ (map (lambda (t) (exp (* -0.3 t))) ts))
         (y-env- (map (lambda (t) (- (exp (* -0.3 t)))) ts)))
    (with-output-to-file output-file
      (lambda ()
        (graph (merge ts ts ts ts ts)
               (merge y-under y-crit y-over y-env+ y-env-)
               #:output-format output-format
               #:top-label "Damped Harmonic Oscillator"
               #:x-label "Time t"
               #:y-label "Displacement x(t)"
               #:x-limits '(0.0 10.0)
               #:y-limits '(-1.1 1.3)
               #:toggle-use-color #t
               #:grid-style 3
               #:line-width 0.004
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))

