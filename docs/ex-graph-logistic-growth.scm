#!/usr/bin/guile \
-e main -s
!#
;;; ex-graph-logistic-growth.scm — Logistic growth and predator-prey dynamics
;;; 1) Logistic growth: dP/dt = r·P·(1 − P/K), solution P(t) = K/(1 + ((K−P₀)/P₀)e^{−rt})
;;; Models population growth with carrying capacity (Verhulst, 1838).
;;; 2) Lotka-Volterra predator-prey: solved via simple Euler integration,
;;; showing the characteristic oscillatory coexistence cycle.
;;; dx/dt = αx − βxy,  dy/dt = δxy − γy

(use-modules (srfi srfi-1)
             (plotutils graph))

;;; --- Logistic growth curves ---
(define (logistic-curve r k p0)
  (lambda (t)
    (/ k (+ 1.0 (* (/ (- k p0) p0) (exp (* (- r) t)))))))

;;; --- Lotka-Volterra via Euler method ---
(define (lotka-volterra alpha beta delta gamma x0 y0 dt n)
  "Return (ts xs ys) for the Lotka-Volterra system."
  (let loop ((i 0) (x x0) (y y0) (ts '()) (xs '()) (ys '()))
    (if (>= i n)
        (list (reverse ts) (reverse xs) (reverse ys))
        (let* ((t (* i dt))
               (dx (* dt (- (* alpha x) (* beta x y))))
               (dy (* dt (- (* delta x y) (* gamma y)))))
          (loop (+ i 1)
                (+ x dx) (+ y dy)
                (cons t ts) (cons x xs) (cons y ys))))))

(define (main args)
  (let* (;; Logistic growth
         (n 400)
         (tmax 10.0)
         (step (/ tmax n))
         (ts (iota n 0.0 step))
         (p1 (map (logistic-curve 1.0 100.0 2.0) ts))
         (p2 (map (logistic-curve 0.5 100.0 2.0) ts))
         (p3 (map (logistic-curve 2.0 100.0 2.0) ts))

         ;; Lotka-Volterra
         (lv (lotka-volterra 1.1 0.4 0.1 0.4 10.0 10.0 0.005 6000))
         (lv-ts (car lv))
         (lv-xs (cadr lv))
         (lv-ys (caddr lv)))

    ;; Plot 1: Logistic growth curves
    (with-output-to-file "graph-logistic-growth.svg"
      (lambda ()
        (graph (merge ts ts ts)
               (merge p1 p2 p3)
               #:output-format "svg"
               #:top-label "Logistic Growth P(t) = K/(1 + ce^{-rt})"
               #:x-label "Time"
               #:y-label "Population"
               #:x-limits '(0.0 10.0)
               #:y-limits '(0.0 110.0)
               #:toggle-use-color #t
               #:grid-style 3
               #:line-width 0.005
               #:font-name "HersheySerif"))
      #:binary #t)

    ;; Plot 2: Lotka-Volterra time series
    (with-output-to-file "graph-lotka-volterra.svg"
      (lambda ()
        (graph (merge lv-ts lv-ts)
               (merge lv-xs lv-ys)
               #:output-format "svg"
               #:top-label "Lotka-Volterra Predator-Prey Dynamics"
               #:x-label "Time"
               #:y-label "Population"
               #:toggle-use-color #t
               #:grid-style 3
               #:line-width 0.004
               #:font-name "HersheySerif"))
      #:binary #t)))
