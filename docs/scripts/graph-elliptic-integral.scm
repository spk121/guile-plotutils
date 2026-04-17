;;; ex-graph-elliptic-integral.scm — Complete elliptic integrals K(k) and E(k)
;;; K(k) = ∫₀^{π/2} dθ / √(1 − k²sin²θ)   (first kind)
;;; E(k) = ∫₀^{π/2} √(1 − k²sin²θ) dθ      (second kind)
;;; These arise in computing arc lengths of ellipses, pendulum periods
;;; (K appears in the exact period of a simple pendulum), and in the
;;; theory of elliptic curves central to modern number theory and cryptography.
;;; K(k) → ∞ as k → 1, while E(1) = 1.

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

(define (elliptic-k k)
  "Complete elliptic integral of the first kind."
  (integrate-simpson
   (lambda (theta) (/ 1.0 (sqrt (- 1.0 (* k k (sin theta) (sin theta))))))
   0.0 (- (/ pi 2.0) 1e-10) 500))

(define (elliptic-e k)
  "Complete elliptic integral of the second kind."
  (integrate-simpson
   (lambda (theta) (sqrt (- 1.0 (* k k (sin theta) (sin theta)))))
   0.0 (/ pi 2.0) 500))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-pendulum-period.svg"))
         (output-format (output-format-from-filename output-file))
         ;; Exact pendulum period T/T_0 = (2/π)K(k) where k = sin(θ_0/2)
         ;; Show for θ_0 from 0 to ~170°
         (n2 300)
         (theta-max (* 170.0 (/ pi 180.0)))
         (theta-step (/ theta-max n2))
         (thetas (iota n2 0.01 theta-step))
         (theta-deg (map (lambda (t) (* t (/ 180.0 pi))) thetas))
         (period-ratio (map (lambda (t)
                              (* (/ 2.0 pi) (elliptic-k (sin (/ t 2.0)))))
                            thetas)))
    ;; Exact pendulum period ratio
    (with-output-to-file output-file
      (lambda ()
        (graph theta-deg period-ratio
               #:output-format output-format
               #:top-label "Exact Pendulum Period T/T\\sb0\\eb vs Amplitude"
               #:x-label "Amplitude \\*H\\sb0\\eb (degrees)"
               #:y-label "T / T\\sb0\\eb"
               #:x-limits '(0.0 170.0)
               #:y-limits '(0.9 4.0)
               #:grid-style 3
               #:line-width 0.005
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
