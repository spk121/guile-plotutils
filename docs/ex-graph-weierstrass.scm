#!/usr/bin/guile \
-e main -s
!#
;;; ex-graph-weierstrass.scm — Weierstrass function
;;; W(x) = Σ_{n=0}^{N} a^n · cos(b^n · π · x)
;;; With 0 < a < 1 and b a positive odd integer where a·b > 1+3π/2,
;;; this function is continuous everywhere but differentiable nowhere.
;;; A pathological example that shattered 19th-century intuitions about
;;; continuity implying differentiability.

(use-modules (srfi srfi-1)
             (plotutils graph))

(define pi (* 4.0 (atan 1.0)))

(define (weierstrass a b n-terms)
  "Return a procedure computing the Weierstrass function with N-TERMS terms."
  (lambda (x)
    (let loop ((n 0) (acc 0.0))
      (if (> n n-terms)
          acc
          (loop (+ n 1)
                (+ acc (* (expt a n) (cos (* (expt b n) pi x)))))))))

(define (main args)
  (let* ((n 2000)
         (xmin -2.0)
         (xmax 2.0)
         (step (/ (- xmax xmin) n))
         (xs (iota n xmin step))
         ;; Classic parameters: a=0.5, b=7
         (y1 (map (weierstrass 0.5 7 40) xs))
         ;; More jagged: a=0.7, b=7
         (y2 (map (weierstrass 0.7 7 30) xs)))
    (with-output-to-file "graph-weierstrass.svg"
      (lambda ()
        (graph (merge xs xs)
               (merge y1 y2)
               #:output-format "svg"
               #:top-label "Weierstrass Function (nowhere differentiable)"
               #:x-label "x"
               #:y-label "W(x)"
               #:x-limits '(-2.0 2.0)
               #:toggle-use-color #t
               #:grid-style 2
               #:line-width 0.002
               #:font-name "HersheySerif"))
      #:binary #t)))
