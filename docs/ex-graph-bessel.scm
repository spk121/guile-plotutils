#!/usr/bin/guile \
-e main -s
!#
;;; ex-graph-bessel.scm — Bessel functions of the first kind J_n(x)
;;; J_n(x) = Σ_{m=0}^{∞} (−1)^m / (m! · Γ(m+n+1)) · (x/2)^(2m+n)
;;; Bessel functions arise as solutions to Laplace's equation in
;;; cylindrical coordinates—fundamental to acoustics of drums, heat
;;; conduction in cylinders, and electromagnetic waveguides.

(use-modules (srfi srfi-1)
             (plotutils graph))

(define (factorial n)
  (if (<= n 1) 1
      (let loop ((i 2) (acc 1))
        (if (> i n) acc
            (loop (+ i 1) (* acc i))))))

(define (bessel-j order x)
  "Compute J_order(x) via power series, for non-negative integer order."
  (let ((terms 25))
    (let loop ((m 0) (acc 0.0))
      (if (> m terms)
          acc
          (let* ((sign (if (even? m) 1.0 -1.0))
                 (num (expt (/ x 2.0) (+ (* 2 m) order)))
                 (den (* (factorial m) (factorial (+ m order)))))
            (loop (+ m 1) (+ acc (/ (* sign num) den))))))))

(define (main args)
  (let* ((n 500)
         (xmin 0.0)
         (xmax 20.0)
         (step (/ (- xmax xmin) n))
         (xs (iota n xmin step))
         (j0 (map (lambda (x) (bessel-j 0 x)) xs))
         (j1 (map (lambda (x) (bessel-j 1 x)) xs))
         (j2 (map (lambda (x) (bessel-j 2 x)) xs))
         (j3 (map (lambda (x) (bessel-j 3 x)) xs)))
    (with-output-to-file "graph-bessel.svg"
      (lambda ()
        (graph (merge xs xs xs xs)
               (merge j0 j1 j2 j3)
               #:output-format "svg"
               #:top-label "Bessel Functions J\\sb0\\eb, J\\sb1\\eb, J\\sb2\\eb, J\\sb3\\eb"
               #:x-label "x"
               #:y-label "J\\sbn\\eb(x)"
               #:x-limits '(0.0 20.0)
               #:y-limits '(-0.5 1.1)
               #:toggle-use-color #t
               #:grid-style 3
               #:line-width 0.004
               #:font-name "HersheySerif"))
      #:binary #t)))
(main (command-line))
